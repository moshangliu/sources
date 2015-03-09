#include "Common.h"
#include "UDPFrame.h"
#include "UDPFrameHelper.h"

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(UDPFrame, test_build_packet) {
    const byte version = VERSION_0;
    const int32 packetId = 1234;
    const byte frameCount = 127;
    const byte frameIndex = 126;
    const int16 contentLen = 10;
    byte* content = new byte[contentLen];
    for (int32 i = 0; i < contentLen; i++) {
        content[i] = rand();
    }

    UDPFrame* frame = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content);
    ASSERT_TRUE(NULL != frame);
    ASSERT_EQ(version, frame->version());
    ASSERT_EQ(UDPPacketType::Packet, frame->packetType());
    ASSERT_EQ(packetId, frame->packetId());
    ASSERT_EQ(frameCount, frame->frameCount());
    ASSERT_EQ(frameIndex, frame->frameIndex());
    ASSERT_EQ(contentLen, frame->contentLength());
    ASSERT_TRUE(same(content, frame->content(), contentLen));
}

TEST(UDPFrame, test_build_ack) {
    const byte version = VERSION_0;
    const int32 packetId = 1234;
    const byte frameCount = 127;
    const byte frameIndex = 126;

    UDPFrame* frame = UDPFrame::buildAck(version, packetId, frameCount, frameIndex);
    ASSERT_TRUE(NULL != frame);
    ASSERT_EQ(version, frame->version());
    ASSERT_EQ(UDPPacketType::Ack, frame->packetType());
    ASSERT_EQ(packetId, frame->packetId());
    ASSERT_EQ(frameCount, frame->frameCount());
    ASSERT_EQ(frameIndex, frame->frameIndex());
    ASSERT_EQ(0, frame->contentLength());
    ASSERT_TRUE(NULL == frame->content());
}

TEST(UDPFrame, test_serialize_packet) {
    const byte version = VERSION_0;
    const int32 packetId = 1234;
    const byte frameCount = 127;
    const byte frameIndex = 126;
    const int16 contentLen = 10;
    byte* content = new byte[contentLen];
    for (int32 i = 0; i < contentLen; i++) {
        content[i] = rand();
    }

    UDPFrame* frame = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content);
    tuple<byte*, int32> result = UDPFrameHelper::serialize(frame);
    byte* data = get<0>(result);
    int32 len = get<1>(result);

    ASSERT_TRUE(NULL != data);
    ASSERT_EQ(UDP_FRAME_HEADER_LEN_FOR_VERSION_0 + frame->contentLength(), len);
    ASSERT_EQ(version, data[0]);
    ASSERT_EQ(UDPPacketType::Packet, data[1]);
    ASSERT_EQ(packetId, toInt32(data[2], data[3], data[4], data[5]));
    ASSERT_EQ(frameCount, data[6]);
    ASSERT_EQ(frameIndex, data[7]);
    ASSERT_EQ(contentLen, toInt16(data[8], data[9]));

    ASSERT_TRUE(same(data + UDP_FRAME_HEADER_LEN_FOR_VERSION_0, frame->content(), frame->contentLength()));
}

TEST(UDPFrame, test_serialize_ack) {
    const byte version = VERSION_0;
    const int32 packetId = 1234;
    const byte frameCount = 127;
    const byte frameIndex = 126;

    UDPFrame* frame = UDPFrame::buildAck(version, packetId, frameCount, frameIndex);
    tuple<byte*, int32> result = UDPFrameHelper::serialize(frame);
    byte* data = get<0>(result);
    int32 len = get<1>(result);

    ASSERT_TRUE(NULL != data);
    ASSERT_EQ(UDP_FRAME_HEADER_LEN_FOR_VERSION_0, len);

    ASSERT_EQ(version, data[0]);
    ASSERT_EQ(UDPPacketType::Ack, data[1]);
    ASSERT_EQ(packetId, toInt32(data[2], data[3], data[4], data[5]));
    ASSERT_EQ(frameCount, data[6]);
    ASSERT_EQ(frameIndex, data[7]);
    ASSERT_EQ(0, toInt16(data[8], data[9]));
}

TEST(UDPFrame, test_unserialize_packet) {
    const byte version = VERSION_0;
    const int32 packetId = 1234;
    const byte frameCount = 127;
    const byte frameIndex = 126;
    const int16 contentLen = 10;
    byte* content = new byte[contentLen];
    for (int32 i = 0; i < contentLen; i++) {
        content[i] = i;
    }

    UDPFrame* frame = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content);
    tuple<byte*, int32> result = UDPFrameHelper::serialize(frame);
    byte* data = get<0>(result);
    int32 len = get<1>(result);

    UDPFrame* frameNew = UDPFrameHelper::unserialize(data, len);
    ASSERT_TRUE(NULL != frameNew);
    ASSERT_EQ(frame->version(), frameNew->version());
    ASSERT_EQ(frame->packetType(), frameNew->packetType());
    ASSERT_EQ(frame->packetId(), frameNew->packetId());
    ASSERT_EQ(frame->frameCount(), frameNew->frameCount());
    ASSERT_EQ(frame->frameIndex(), frameNew->frameIndex());
    ASSERT_EQ(frame->contentLength(), frameNew->contentLength());
    ASSERT_TRUE(same(frame->content(), frameNew->content(), contentLen));
}

void test_segment(byte* data, int dataLen);

TEST(UDPFrame, test_segment_null) {
    test_segment(NULL, 0);
}

TEST(UDPFrame, test_segment_half_packet) {
    int dataLen = UDP_FRAME_MAX_SIZE / 2;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }
    test_segment(data, dataLen);
}

TEST(UDPFrame, test_segment_one_packet) {
    int dataLen = UDP_FRAME_MAX_SIZE;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }
    test_segment(data, dataLen);
}

TEST(UDPFrame, test_segment_one_half_packet) {
    int dataLen = UDP_FRAME_MAX_SIZE * 3 / 2;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }
    test_segment(data, dataLen);
}

TEST(UDPFrame, test_segment_two_packet) {
    int dataLen = UDP_FRAME_MAX_SIZE * 2;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }
    test_segment(data, dataLen);
}

TEST(UDPFrame, test_segment_max_packet_notfull) {
    int dataLen = UDP_PACKET_MAX_SIZE - (UDP_FRAME_MAX_SIZE / 2);
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }
    test_segment(data, dataLen);
}

TEST(UDPFrame, test_segment_max_packet_full) {
    int dataLen = UDP_PACKET_MAX_SIZE;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }
    test_segment(data, dataLen);
}

TEST(UDPFrame, test_segment_over_max_packet) {
    int dataLen = UDP_PACKET_MAX_SIZE + 1;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }
    test_segment(data, dataLen);
}

void test_segment(byte* data, int dataLen) {
    vector<UDPFrame*>* frames = UDPFrameHelper::segment(data, dataLen);
    if (data == NULL || dataLen <= 0 || dataLen > UDP_PACKET_MAX_SIZE) {
        ASSERT_TRUE(frames == NULL);
        return;
    }
    ASSERT_TRUE(frames != NULL);

    int frameCount = dataLen / UDP_FRAME_MAX_SIZE;
    if (dataLen % UDP_FRAME_MAX_SIZE > 0) {
        frameCount ++;
    }
    ASSERT_TRUE(frameCount > 0);
    ASSERT_EQ(frameCount, frames->size());

    int packetId = frames->at(0)->packetId();
    for (int i = 0; i < frameCount; i++) {
        UDPFrame* frame = frames->at(i);
        ASSERT_EQ(VERSION_0, frame->version());
        ASSERT_EQ(UDPPacketType::Packet, frame->packetType());
        ASSERT_EQ(packetId, frame->packetId());
        ASSERT_EQ(frameCount, frame->frameCount());
        ASSERT_EQ(i + 1, frame->frameIndex());

        int contentLenExpected = i == frameCount - 1 ? dataLen - i*UDP_FRAME_MAX_SIZE : UDP_FRAME_MAX_SIZE;
        ASSERT_EQ(contentLenExpected, frame->contentLength());

        same(data + i*UDP_FRAME_MAX_SIZE, frame->content(), frame->contentLength());
    }
}
