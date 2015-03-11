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
    ASSERT_TRUE(same(data, get<0>(result), dataLen));
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
    ASSERT_TRUE(same(data, get<0>(result), dataLen));
    ASSERT_EQ(dataLen, get<1>(result));
}


TEST(UDPRecvContainer, test_UDPRecvContainer_putOrAssemble_one_frame) {
    string ip = "127.0.0.1";
    int port = 1234;

    int dataLen = UDP_FRAME_MAX_SIZE;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }

    vector<UDPFrame*>* frames = UDPFrameHelper::segment(data, dataLen);
    ASSERT_EQ(1, frames->size());

    UDPRecvContainer* _recvContainer = new UDPRecvContainer();
    tuple<bool, byte*, int> t = _recvContainer->putOrAssemble(ip, port, frames->at(0));
    ASSERT_TRUE(get<0>(t));
    ASSERT_TRUE(get<1>(t) != NULL);
    ASSERT_EQ(dataLen, get<2>(t));
    ASSERT_TRUE(same(get<1>(t), data, dataLen));
}

TEST(UDPRecvContainer, test_UDPRecvContainer_putOrAssemble_two_frame) {
    string ip = "127.0.0.1";
    int port = 1234;

    int dataLen = UDP_FRAME_MAX_SIZE * 2;
    byte* data = new byte[dataLen];
    for (int i = 0; i < dataLen; i++) {
        data[i] = rand();
    }

    vector<UDPFrame*>* frames = UDPFrameHelper::segment(data, dataLen);
    ASSERT_EQ(2, frames->size());

    UDPRecvContainer* _recvContainer = new UDPRecvContainer();
    tuple<bool, byte*, int> t = _recvContainer->putOrAssemble(ip, port, frames->at(0));
    ASSERT_FALSE(get<0>(t));
    ASSERT_TRUE(get<1>(t) == NULL);
    ASSERT_EQ(0, get<2>(t));

//    t = _recvContainer->putOrAssemble(ip, port, frames->at(0));
//    ASSERT_FALSE(get<0>(t));
//    ASSERT_TRUE(get<1>(t) == NULL);
//    ASSERT_EQ(0, get<2>(t));

    t = _recvContainer->putOrAssemble(ip, port, frames->at(1));
    ASSERT_TRUE(get<0>(t));
    ASSERT_TRUE(get<1>(t) != NULL);
    ASSERT_EQ(dataLen, get<2>(t));
    ASSERT_TRUE(same(get<1>(t), data, dataLen));
}
