#include "Common.h"

#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;


const string EMPTY = "";

const int VERSION_0 = 0;
const int UDP_FRAME_HEADER_LEN_FOR_VERSION_0 = 10;
const int UDP_FRAME_MAX_SIZE = 1025;
const int UDP_FRAME_MAX_COUNT = 127;
const int UDP_PACKET_MAX_SIZE = UDP_FRAME_MAX_SIZE * UDP_FRAME_MAX_COUNT;

const int ZERO_WAIT_TIME_US = 0;
const int ONE_WAIT_TIME_US = 10*1000;
const int TWO_WAIT_TIME_US = 50*1000;
const int THREE_WAIT_TIME_US =  100*1000;
const int FOUR_WAIT_TIME_US =  500*1000;
const int FIVE_WAIT_TIME_US =  1000*1000;
const int SIX_WAIT_TIME_US =  2000*1000;

int split(string str, string delim, vector<string>& parts)
{
    if (delim.length() == 0)
    {
        return -1;
    }

    size_t idx;
    while ((idx = str.find(delim)) != string::npos)
    {
        parts.push_back(str.substr(0, idx));
        str = str.substr(idx + delim.length());
    }

    if (str.size() > 0)
    {
        parts.push_back(str);
    }
    return 0;
}

string intToStr(int num)
{
    const int MAX_LEN = 20;
    char buf[MAX_LEN];
    memset(buf, 0, MAX_LEN);

    sprintf(buf, "%d", num);
    return std::string(buf);
}

string ltrim(string str, string rmv)
{
    uint32 idx = 0;
    for (idx = 0; idx < str.size(); idx++)
    {
        if (rmv.find(string(&(str[idx]), 1)) == string::npos)
        {
            break;
        }
    }

    return idx == str.size() ? "" : str.substr(idx, str.size() - idx);
}

string rtrim(string str, string rmv)
{
    if (str.size() == 0)
    {
        return EMPTY;
    }
    int32 idx = str.size() - 1;
    for (idx = str.size() - 1; idx >= 0; idx--)
    {
        if (rmv.find(string(&(str[idx]), 1)) == string::npos)
        {
            break;
        }
    }

    return idx == -1 ? EMPTY : str.substr(0, idx + 1);
}

string trim(string str, string rmv)
{
    return rtrim(ltrim(str, rmv), rmv);
}

string ip(struct sockaddr_in* addr)
{
    const int IP_STR_LEN = 32;
    char ipStr[IP_STR_LEN];
    memset(ipStr, 0, IP_STR_LEN);

    return inet_ntop(AF_INET, (void*)(&(addr->sin_addr)), ipStr, IP_STR_LEN) != NULL
        ? string(ipStr)
        : EMPTY;

}

uint16 port(struct sockaddr_in* addr)
{
    return ntohs(addr->sin_port);
}

string toString(int value, int maxLen)
{
    int rt = 0;
    char str[255];
    if (maxLen <= 0 || maxLen > 30)
    {
        return EMPTY;
    }

    rt = snprintf(str, maxLen + 1, "%d", value);
    if (rt > maxLen)
    {
        return EMPTY;
    }
    str[rt] = '\0';
    return string(str);
}

tuple<byte, byte, byte, byte> int32To4Bytes(int32 d) {
    const byte b1 = (d >> 24) & 0x000000FF;
    const byte b2 = (d >> 16) & 0x000000FF;
    const byte b3 = (d >> 8) & 0x000000FF;
    const byte b4 = (d) & 0x000000FF;

    return make_tuple(b1, b2, b3, b4);
}

int32 toInt32(byte b1, byte b2, byte b3, byte b4) {
    int result = 0;

    int b = b1;
    b = b & 0x000000FF;
    result |= b;
    result = result << 8;

    b = b2;
    b = b & 0x000000FF;
    result |= b;
    result = result << 8;

    b = b3;
    b = b & 0x000000FF;
    result |= b;
    result = result << 8;

    b = b4;
    b = b & 0x000000FF;
    result |= b;

    return result;
}

std::tuple<byte, byte> int16To2Bytes(int16 d) {
    const byte b1 = (d >> 8) & 0x000000FF;
    const byte b2 = (d) & 0x000000FF;

    return make_tuple(b1, b2);
}

int16 toInt16(byte b1, byte b2) {
    int result = 0;

    int b = b1;
    b = b & 0x000000FF;
    result |= b;
    result = result << 8;

    b = b2;
    b = b & 0x000000FF;
    result |= b;

    return result;
}

long current_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

string makeKey(int packetId, byte frameIndex) {
    return format("%d-%d", packetId, frameIndex);
}

string makeRecvPacketKey(const string& ip, int port, int packetId) {
    return format("%s:%d-%d", ip.c_str(), port, packetId);
}

string format(const char* format, ...) {
    const int LOG_MAX_LEN = 1024;
    char buf[LOG_MAX_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, LOG_MAX_LEN, format, args);
    va_end(args);

    return string(buf);
}

bool same(const byte* data1, const byte* data2, int32 len) {
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
