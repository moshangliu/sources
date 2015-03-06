#include "UDPFrame.h"
#include "UDPFrameHelper.h"
#include "UDPRecvDataStructs.h"

#include <cstdlib>
#include <gtest/gtest.h>

using namespace std;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(UDPRecvObj, test_UDPRecvObj_put) {
    string ip = "127.0.0.1";
    int port = 10000;

    byte version = VERSION_0;
    int32 packetId = 123;
    byte frameCount = 2;
    int16 contentLen = 10;
    byte* content1 = new byte[contentLen];
    for (int16 i = 0; i < contentLen; i++) {
        content1[i] = i;
    }

    byte* content2 = new byte[contentLen];
    for (int16 i = 0; i < contentLen; i++) {
        content2[i] = rand();
    }

    byte frameIndex = 1;
    UDPFrame* frame1 = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content1);

    frameIndex = 2;
    UDPFrame* frame2 = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content2);

    UDPRecvObj recvObj(ip, port, packetId, frameCount);
    ASSERT_FALSE(recvObj.completed());

    ASSERT_EQ(ip, recvObj.ip());
    ASSERT_EQ(port, recvObj.port());
    ASSERT_EQ(packetId, recvObj.packetId());
    ASSERT_EQ(frameCount, recvObj.frameCount());

    recvObj.put(frame1);
    ASSERT_FALSE(recvObj.completed());

    recvObj.put(frame2);
    ASSERT_TRUE(recvObj.completed());
}

bool assertSame(const byte* data1, const byte* data2, int32 len);

TEST(UDPRecvObj, test_UDPRecvObj_assemble_one_frame) {
    int dataLen = UDP_FRAME_MAX_SIZE;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }

    vector<UDPFrame*>* frames = UDPFrameHelper::segment(data, dataLen);
    ASSERT_EQ(1, frames->size());

    string ip = "127.0.0.1";
    int port = 10000;
    UDPFrame* frame = frames->at(0);
    UDPRecvObj recvObj(ip, port, frame->packetId(), frame->frameCount());

    recvObj.put(frame);
    ASSERT_TRUE(recvObj.completed());

    tuple<byte*, int> result = recvObj.assemble();
    ASSERT_TRUE(assertSame(data, get<0>(result), dataLen));
    ASSERT_EQ(dataLen, get<1>(result));
}

TEST(UDPRecvObj, test_UDPRecvObj_assemble_multi_frames_not_finish) {
    string ip = "127.0.0.1";
    int port = 10000;

    int dataLen = UDP_PACKET_MAX_SIZE;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }

    vector<UDPFrame*>* frames = UDPFrameHelper::segment(data, dataLen);
    ASSERT_EQ(UDP_PACKET_MAX_SIZE / UDP_FRAME_MAX_SIZE, frames->size());

    UDPFrame* frame1 = frames->at(0);
    UDPRecvObj recvObj(ip, port, frame1->packetId(), frame1->frameCount());
    recvObj.put(frame1);
    ASSERT_FALSE(recvObj.completed());

    tuple<byte*, int> result = recvObj.assemble();
    ASSERT_EQ(NULL, get<0>(result));
    ASSERT_EQ(0, get<1>(result));
}

TEST(UDPRecvObj, test_UDPRecvObj_assemble_multi_frames_finish) {
    string ip = "127.0.0.1";
    int port = 10000;

    const int dataLen = UDP_PACKET_MAX_SIZE;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }

    vector<UDPFrame*>* frames = UDPFrameHelper::segment(data, dataLen);
    ASSERT_EQ(UDP_PACKET_MAX_SIZE / UDP_FRAME_MAX_SIZE, frames->size());

    UDPFrame* frame1 = frames->at(0);
    UDPRecvObj recvObj(ip, port, frame1->packetId(), frame1->frameCount());
    ASSERT_FALSE(recvObj.completed());

    for (vector<UDPFrame*>::iterator it = frames->begin(); it != frames->end(); it++) {
        recvObj.put(*it);
    }
    ASSERT_TRUE(recvObj.completed());
    tuple<byte*, int> result = recvObj.assemble();
    ASSERT_TRUE(assertSame(data, get<0>(result), dataLen));
    ASSERT_EQ(dataLen, get<1>(result));
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
