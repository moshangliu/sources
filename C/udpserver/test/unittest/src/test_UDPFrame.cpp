#include "UDPFrame.h"
#include "UDPFrameHelper.h"

#include <gtest/gtest.h>

bool assertSame(const byte* data1, const byte* data2, int32 len);

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
        content[i] = i;
    }

    UDPFrame* frame = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content);
    ASSERT_TRUE(NULL != frame);
    ASSERT_EQ(version, frame->version());
    ASSERT_EQ(UDPPacketType::Packet, frame->packetType());
    ASSERT_EQ(packetId, frame->packetId());
    ASSERT_EQ(frameCount, frame->frameCount());
    ASSERT_EQ(frameIndex, frame->frameIndex());
    ASSERT_EQ(contentLen, frame->contentLength());
    ASSERT_TRUE(assertSame(content, frame->content(), contentLen));
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
        content[i] = i;
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

    ASSERT_TRUE(assertSame(data + UDP_FRAME_HEADER_LEN_FOR_VERSION_0, frame->content(), frame->contentLength()));
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

bool assertSame(const byte* data1, const byte* data2, int32 len) {
    if (data1 == NULL && data2 == NULL) {
        return true;
    }

    for (int32 i = 0; i < len; i++) {
        if (data1[i] != data2[i]) {
            return false;
        }
    }

    return true;
}
