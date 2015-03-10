#include "UDPFrame.h"
#include "UDPSendDataStructs.h"

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(UDPResendObj, test_UDPResendObj) {
    string ip = "127.0.0.1";
    int port = 1234;
    byte version = VERSION_0;
    int32 packetId = 123;
    byte frameCount = 1;
    byte frameIndex = 1;
    int16 contentLen = 10;
    byte* content = new byte[contentLen];
    for (int16 i = 0; i < contentLen; i++) {
        content[i] = i;
    }

    UDPFrame* frame = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content);

    UDPResendObj* obj = new UDPResendObj(ip, port, frame);
    ASSERT_EQ(ip, obj->ip());
    ASSERT_EQ(port, obj->port());
    long currentUs = current_us();
    ASSERT_TRUE((currentUs - obj->sendTsUs()) > 0);
    ASSERT_TRUE((currentUs - obj->sendTsUs()) < 1000);
    ASSERT_EQ(0, obj->triedCnt());

    long sendTsUs = obj->sendTsUs();
    obj->update();

    ASSERT_EQ(1, obj->triedCnt());
    ASSERT_EQ(ONE_WAIT_TIME_US, obj->sendTsUs() - sendTsUs);
    sendTsUs = obj->sendTsUs();

    obj->update();

    ASSERT_EQ(2, obj->triedCnt());
    ASSERT_EQ(TWO_WAIT_TIME_US, obj->sendTsUs() - sendTsUs);
    sendTsUs = obj->sendTsUs();

    obj->update();

    ASSERT_EQ(3, obj->triedCnt());
    ASSERT_EQ(THREE_WAIT_TIME_US, obj->sendTsUs() - sendTsUs);
    sendTsUs = obj->sendTsUs();

    obj->update();

    ASSERT_EQ(4, obj->triedCnt());
    ASSERT_EQ(FOUR_WAIT_TIME_US, obj->sendTsUs() - sendTsUs);
    sendTsUs = obj->sendTsUs();

    obj->update();

    ASSERT_EQ(5, obj->triedCnt());
    ASSERT_EQ(FIVE_WAIT_TIME_US, obj->sendTsUs() - sendTsUs);
    sendTsUs = obj->sendTsUs();

    obj->update();

    ASSERT_EQ(6, obj->triedCnt());
    ASSERT_EQ(SIX_WAIT_TIME_US, obj->sendTsUs() - sendTsUs);
}

TEST(UDPResendQueue, test_UDPResendQueue) {
    string ip = "127.0.0.1";
    int port = 1234;

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
        content2[i] = i;
    }

    byte frameIndex = 1;
    UDPFrame* frame1 = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content1);
    UDPResendObj* obj1 = new UDPResendObj(ip, port, frame1);

    frameIndex = 2;
    UDPFrame* frame2 = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content2);
    UDPResendObj* obj2 = new UDPResendObj(ip, port, frame2);

    UDPResendQueue::instance()->push(obj1);
    UDPResendQueue::instance()->push(obj2);

    UDPResendObj* obj = UDPResendQueue::instance()->top();
    ASSERT_EQ(obj->frame()->frameIndex(), obj1->frame()->frameIndex());

    UDPResendQueue::instance()->pop();
    obj->update();
    UDPResendQueue::instance()->push(obj);

    obj = UDPResendQueue::instance()->top();
    ASSERT_EQ(obj->frame()->frameIndex(), obj2->frame()->frameIndex());
    UDPResendQueue::instance()->pop();

    obj->update();
    UDPResendQueue::instance()->push(obj);
    obj = UDPResendQueue::instance()->top();
    ASSERT_EQ(obj->frame()->frameIndex(), obj1->frame()->frameIndex());
}

TEST(UDPAckMap, test_UDPAckMap) {
    ASSERT_EQ(0, UDPAckMap::instance()->size());

    int packetId = 1234;
    byte frameIndex = 1;

    UDPAckMap::instance()->setNotAcked(packetId, frameIndex);
    ASSERT_TRUE(UDPAckMap::instance()->needResend(packetId, frameIndex));
    ASSERT_EQ(1, UDPAckMap::instance()->size());

    UDPAckMap::instance()->setAckedIfExist(packetId, frameIndex);
    ASSERT_FALSE(UDPAckMap::instance()->needResend(packetId, frameIndex));
    ASSERT_EQ(1, UDPAckMap::instance()->size());

    UDPAckMap::instance()->erase(packetId, frameIndex);
    ASSERT_FALSE(UDPAckMap::instance()->needResend(packetId, frameIndex));
    ASSERT_EQ(0, UDPAckMap::instance()->size());

    UDPAckMap::instance()->erase(packetId, frameIndex);
    ASSERT_FALSE(UDPAckMap::instance()->needResend(packetId, frameIndex));
    ASSERT_EQ(0, UDPAckMap::instance()->size());
}
