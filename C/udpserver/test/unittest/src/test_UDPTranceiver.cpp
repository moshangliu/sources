#include "Common.h"
#include "UDPFrame.h"
#include "UDPFrameHelper.h"
#include "UDPTranceiver.h"
#include "LoggerWrapper.h"
#include "UDPPacketReceivedHandler.h"
#include "UDPRetryTimeSpan.h"

#include "boost/shared_array.hpp"
#include <tuple>
#include <vector>
#include <gtest/gtest.h>
#include <unistd.h>

using namespace std;
using namespace boost;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

const byte TEST_TYPE = 0;
class UDPTranceiverTestHandler : public UDPPacketReceivedHandler {
private:
    // IP, PORT, CONTENT, CONTENT_LEN
    vector<tuple<string, int, byte*, int>> _contents;

public:
    virtual void handle(string ip, int port, byte* content, int contentLen) {
        LoggerWrapper::instance()->info("UT, RecvFrom:%s-%d, contentLen:%d",
            ip.c_str(), port, contentLen);

        _contents.push_back(make_tuple(ip, port, content, contentLen));
    }

    ~UDPTranceiverTestHandler() {}

    vector<tuple<string, int, byte*, int>>& contents() {
        return _contents;
    }
};

class UDPSendSuccessHandler4Test : public UDPPacketSendSuccessHandler {
private:
    // IP, PORT, TYPE, CONTENT, CONTENT_LEN
    vector<tuple<string, int, byte, byte*, int>> _contents;

public:
    virtual void handle(string ip, int port, byte type, byte* content, int contentLen) {
        LoggerWrapper::instance()->info("UT, SEND_PACKET_SUCCESS, To:%s-%d, contentLen:%d",
            ip.c_str(), port, contentLen);
        _contents.push_back(make_tuple(ip, port, type, content, contentLen));
    }

    ~UDPSendSuccessHandler4Test() {}

    vector<tuple<string, int, byte, byte*, int>>& contents() {
        return _contents;
    }
};

class UDPSendFailureHandler4Test : public UDPPacketSendFailureHandler {
private:
    // IP, PORT, TYPE, CONTENT, CONTENT_LEN
    vector<tuple<string, int, byte, byte*, int>> _contents;

public:
    virtual void handle(string ip, int port, byte type, byte* content, int contentLen) {
        LoggerWrapper::instance()->info("UT, SEND_PACKET_FAILURE, To:%s-%d, contentLen:%d",
            ip.c_str(), port, contentLen);
        _contents.push_back(make_tuple(ip, port, type, content, contentLen));
    }

    ~UDPSendFailureHandler4Test() {}

    vector<tuple<string, int, byte, byte*, int>>& contents() {
        return _contents;
    }
};

void testSendSelf(char* content, int contentLen);
static int16 selfPort = 10000;

TEST(UDPTranceiver, test_UDPTranceiver_self_1_frame) {

    int contentLen = UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendSelf(content.get(), contentLen);
}

TEST(UDPTranceiver, test_UDPTranceiver_self_2_frame) {

    int contentLen = 2*UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendSelf(content.get(), contentLen);
}

TEST(UDPTranceiver, test_UDPTranceiver_self_4_frame) {

    int contentLen = 4*UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendSelf(content.get(), contentLen);
}

TEST(UDPTranceiver, test_UDPTranceiver_self_max_frame) {

    int contentLen = UDP_FRAME_MAX_COUNT*UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendSelf(content.get(), contentLen);
}


int16 fromPort = 11000;
int16 toPort = 12000;
void testSendOther(char* content, int contentLen);

TEST(UDPTranceiver, test_UDPTranceiver_other_1_frame) {

    int contentLen = UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendOther(content.get(), contentLen);
}

TEST(UDPTranceiver, test_UDPTranceiver_other_2_frame) {

    int contentLen = 2*UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendOther(content.get(), contentLen);
}

TEST(UDPTranceiver, test_UDPTranceiver_other_4_frame) {

    int contentLen = 4*UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendOther(content.get(), contentLen);
}

TEST(UDPTranceiver, test_UDPTranceiver_other_max_frame) {

    int contentLen = UDP_FRAME_MAX_COUNT*UDP_FRAME_MAX_SIZE - 1;
    shared_array<char> content (new char[contentLen]);
    for (int32 i = 0; i < contentLen; i++) {
        content.get()[i] = (char)rand();
    }

    testSendOther(content.get(), contentLen);
}

void testSendSelf(char* content, int contentLen) {
    string host = "127.0.0.1";
    selfPort++;

    UDPTranceiverTestHandler* handler = new UDPTranceiverTestHandler();
    UDPSendSuccessHandler4Test* successHandler = new UDPSendSuccessHandler4Test();
    UDPSendFailureHandler4Test* failureHandler = new UDPSendFailureHandler4Test();

    UDPTranceiver* tranceiver = new UDPTranceiver(selfPort);
    tranceiver->registerHandler(TEST_TYPE, handler);
    tranceiver->setSuccessHandler(successHandler);
    tranceiver->setFailureHandler(failureHandler);

    tranceiver->run();

    tranceiver->send(TEST_TYPE, host, selfPort, content, contentLen);
    sleep(2);

    // Check recvHandler
    vector<tuple<string, int, byte*, int>>& contents = handler->contents();
    ASSERT_EQ(1, contents.size());

    tuple<string, int, byte*, int> result = contents[0];
    ASSERT_EQ(host, get<0>(result));
    ASSERT_EQ(selfPort, get<1>(result));
    ASSERT_EQ(contentLen, get<3>(result));
    ASSERT_TRUE(same((byte*)content, get<2>(result), contentLen));

//    // Check successHandler
//    vector<tuple<string, int, byte, byte*, int>>& successContents = successHandler->contents();
//    ASSERT_EQ(1, successContents.size());
//
//    tuple<string, int, byte, byte*, int> successResult = successContents[0];
//    ASSERT_EQ(host, get<0>(result));
//    ASSERT_EQ(selfPort, get<1>(result));
//    ASSERT_EQ(TEST_TYPE, get<2>(result));
//    ASSERT_EQ(contentLen, get<4>(result));
//    ASSERT_TRUE(same((byte*)content, get<3>(result), contentLen));
//
//    // Check failureHandler
//    vector<tuple<string, int, byte, byte*, int>>& failureContents = failureHandler->contents();
//    ASSERT_EQ(0, failureContents.size());

    tranceiver->stop();

    int waitThreadStopSec = UDPRetryTimeSpan::instance()->allWaitTimeUs() / 1000 / 1000 + 1;
    sleep(waitThreadStopSec);
    sleep(1);
}

void testSendOther(char* content, int contentLen) {
    string host = "127.0.0.1";
    fromPort++;
    toPort++;

    UDPTranceiver* fromTranceiver = new UDPTranceiver(fromPort);
    UDPTranceiver* toTranceiver = new UDPTranceiver(toPort);
    UDPTranceiverTestHandler* handler = new UDPTranceiverTestHandler();
    toTranceiver->registerHandler(TEST_TYPE, handler);

    fromTranceiver->run();
    toTranceiver->run();
    sleep(1);

    fromTranceiver->send(TEST_TYPE, host, toPort, content, contentLen);
    sleep(2);

    vector<tuple<string, int, byte*, int>>& contents = handler->contents();
    ASSERT_EQ(1, contents.size());

    tuple<string, int, byte*, int> result = contents[0];
    ASSERT_EQ(host, get<0>(result));
    ASSERT_EQ(fromPort, get<1>(result));
    ASSERT_EQ(contentLen, get<3>(result));
    ASSERT_TRUE(same((byte*)content, get<2>(result), contentLen));

    fromTranceiver->stop();
    toTranceiver->stop();

    int waitThreadStopSec = UDPRetryTimeSpan::instance()->allWaitTimeUs() / 1000 / 1000 + 1;
    sleep(waitThreadStopSec);

    sleep(1);
}
