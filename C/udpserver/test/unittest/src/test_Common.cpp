#include "Common.h"

#include <iostream>
#include <arpa/inet.h>
#include<fcntl.h>
#include <sys/stat.h>
#include <set>
#include <map>
#include <queue>

#include <gtest/gtest.h>

using namespace std;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

const string WHITE_SPACES = " \r\n";

TEST(ltrim, test_ltrim) {
    ASSERT_EQ(ltrim("hello", WHITE_SPACES), "hello");
    ASSERT_EQ(ltrim(" hello", WHITE_SPACES), "hello");
    ASSERT_EQ(ltrim("  hello", WHITE_SPACES), "hello");
    ASSERT_EQ(ltrim("  hello ", WHITE_SPACES), "hello ");
    ASSERT_EQ(ltrim("    ", WHITE_SPACES), EMPTY);
    ASSERT_EQ(ltrim(EMPTY, WHITE_SPACES), EMPTY);
}

TEST(rtrim, test_rtrim) {
    ASSERT_EQ(rtrim("hello", WHITE_SPACES), "hello");
    ASSERT_EQ(rtrim("hello ", WHITE_SPACES), "hello");
    ASSERT_EQ(rtrim("  hello", WHITE_SPACES), "  hello");
    ASSERT_EQ(rtrim("  hello ", WHITE_SPACES), "  hello");
    ASSERT_EQ(rtrim("    ", WHITE_SPACES), "");
}

TEST(trim, test_trim) {
    ASSERT_EQ(trim("hello", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("hello ", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("  hello", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("  hello ", WHITE_SPACES), "hello");
    ASSERT_EQ(trim("    ", WHITE_SPACES), "");
}

TEST(int32To4Bytes, test_int32To4Bytes) {
    int32 d = 0xFAFBFCFD;
    tuple<byte, byte, byte, byte> parts = int32To4Bytes(d);
    ASSERT_EQ(0xFA, get<0>(parts));
    ASSERT_EQ(0xFB, get<1>(parts));
    ASSERT_EQ(0xFC, get<2>(parts));
    ASSERT_EQ(0xFD, get<3>(parts));
}

TEST(toInt32, test_toInt32) {
    byte b1 = 0xFA;
    byte b2 = 0xFB;
    byte b3 = 0xFC;
    byte b4 = 0xFD;

    ASSERT_EQ(0xFAFBFCFD, toInt32(b1, b2, b3, b4));
}

TEST(int16To4Bytes, test_int16To2Bytes) {
    int16 d = 0xFAFB;
    tuple<byte, byte> parts = int16To2Bytes(d);
    ASSERT_EQ(0xFA, get<0>(parts));
    ASSERT_EQ(0xFB, get<1>(parts));
}

TEST(toInt16, test_toInt16) {
    byte b1 = 0xFA;
    byte b2 = 0xFB;

    ASSERT_EQ((int16)0xFAFB, toInt16(b1, b2));
}

TEST(current_us, test_current_us) {
    long now = current_us();
    cout << "Now(us): " << now << endl;
}

TEST(format, test_format) {
    int packetId = 1234;
    byte frameIndex = 12;

    string key = format("%d-%d", packetId, frameIndex);
    ASSERT_EQ("1234-12", key);
}

TEST(makeKey, test_makeKey) {
    int packetId = 1234;
    byte frameIndex = 12;

    string key = makeKey(packetId, frameIndex);
    ASSERT_EQ("1234-12", key);
}

TEST(makeRecvPacketKey, test_makeRecvPacketKey) {
    string ip = "127.0.0.1";
    int packetId = 1234;
    byte frameIndex = 12;

    string key = makeRecvPacketKey(ip, packetId, frameIndex);
    ASSERT_EQ("127.0.0.1:1234-12", key);
}

TEST(ip2in_addr, test_ip2in_addr) {
    string ip = "192.168.1.1";

    in_addr addr;
    addr.s_addr = inet_addr(ip.c_str());
    string newIP = string(inet_ntoa(addr));

    ASSERT_EQ(ip, newIP);

}

TEST(set, test_set) {
    byte b = 12;
    set<byte> data;

    data.insert(b);
    ASSERT_EQ(1, data.size());

    data.erase(b);
    ASSERT_EQ(0, data.size());

    data.erase(b);
    ASSERT_EQ(0, data.size());

}

TEST(map, test_set) {
    int key = 12;
    string value = "value";
    map<int, string> data;

    data[key] = value;
    ASSERT_EQ(1, data.size());

    data.erase(key);
    ASSERT_EQ(0, data.size());

    data.erase(key);
    ASSERT_EQ(0, data.size());
}
