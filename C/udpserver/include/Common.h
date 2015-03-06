#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <atomic>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned char byte;

extern const std::string EMPTY;

extern const int VERSION_0;
extern const int UDP_FRAME_HEADER_LEN_FOR_VERSION_0;
extern const int UDP_FRAME_MAX_SIZE;
extern const int UDP_FRAME_MAX_COUNT;
extern const int UDP_PACKET_MAX_SIZE;

extern const int ZERO_WAIT_TIME_US;
extern const int ONE_WAIT_TIME_US;
extern const int TWO_WAIT_TIME_US;
extern const int THREE_WAIT_TIME_US;
extern const int FOUR_WAIT_TIME_US;
extern const int FIVE_WAIT_TIME_US;
extern const int SIX_WAIT_TIME_US;

/**
 * @brief: split [str] with [delim] and store in [parts]
 * @param [in] str: string splited
 * @param [in] delim: delimeter
 * @param [out] parts: parts splited
 * @returen: success 0, failure: -1
 * e.g
 *      Split("part1|part2", "|", vector)
 *      => ["part1", "part2"]
 **/
int split(std::string str, std::string delim,
    std::vector<std::string>& parts);

/**
 * @brief: Int to string
 * @param [in] num: int
 * @return: string
 * e.g
 *      123 --> "123"
 *     -123 --> "-123"
 **/
std::string intToStr(int num);

/**
 * @brief: left trim
 * @param [in] str: original string
 * @param [in] rmv: char set removed
 * @return: string left trimmed
 **/
std::string ltrim(std::string str, std::string rmv);

/**
 * @brief: right trim
 * @param [in] str: original string
 * @param [in] rmv: char set removed
 * @return: string right trimmed
 **/
std::string rtrim(std::string str, std::string rmv);

/**
 * @brief: trim
 * @param [in] str: original string
 * @param [in] rmv: char set removed
 * @return: string trimmed
 **/
std::string trim(std::string str, std::string rmv);

/**
 * @brief: Parse IP-string from struct sockaddr_in
 * @param [in] addr: struct sockaddr_in
 * @return: IP-string
 **/
std::string ip(struct sockaddr_in* addr);

uint16 port(struct sockaddr_in* addr);

/**
 * @brief: convert value to string
 * @param [in] value: integer
 * @param [in] maxLen: max int len
 **/
std::string toString(int value, int maxLen = 12);

std::tuple<byte, byte, byte, byte> int32To4Bytes(int32 d);

int32 toInt32(byte b1, byte b2, byte b3, byte b4);

std::tuple<byte, byte> int16To2Bytes(int16 d);

int16 toInt16(byte b1, byte b2);

long current_us();

std::string makeKey(int packetId, byte frameIndex);
#endif
