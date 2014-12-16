#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <vector>
#include <map>

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

class HttpResponse;

extern const std::string HTTP_1_0;
extern const std::string HTTP_1_1;

extern const std::string EMPTY;
extern const std::string STREAM_TYPE;

extern const char* SP;
extern const char* CRLF;
extern const char* CRLFCRLF;
extern const char* SPLIT;
extern const char* DOT;

enum HTTP_METHOD
{
    GET,
    POST,
    HEAD,
    PUT,
    DELETE,
    TRACE,
    CONNECT,
    OPTIONS,
    OTHER  // Error
};

HTTP_METHOD GetHttpMethod(std::string method);

std::string HttpMethodToString(HTTP_METHOD method);

int readn(int32 fd, char* buf, uint32 nbytes);

int writen(int32 fd, const char* buf, uint32 nbytes);

int readLine(int32 fd, char* buf, uint32 bufSize);

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
int Split(std::string str, std::string delim,
    std::vector<std::string>& parts);

/**
 * @brief: Create CString length = [len]
 * @param [in] len: length of CString created
 * @return: CString_inited by 0 for each byte
 *      length is [len]
 **/
char* CreateCStr(const ulong len);

/**
 * @brief: write http response to fd
 * @param [in] fd: file description written to
 * @param [in] response: http response
 * @return: success 0, failure -1
 **/
int WriteResponse(int fd, HttpResponse& response);

/**
 * @brief: Int to string
 * @param [in] num: int
 * @return: string
 * e.g
 *      123 --> "123"
 *     -123 --> "-123"
 **/
std::string IntToStr(int num);

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
 * @brief: Get file extension by file name
 * @param [in] path: file path
 * @return: file extension
 * e.g.
 *      /index.html --> html
 **/
std::string GetFileExtension(std::string path);

/**
 * @brief: Get pure url
 * @param [in] url: full url with GET parameters
 * @return: pure url without paramters
 * e.g
 *      [in] /index.html?usr=test&pwd=test
 *      [return] /index.html
 *
 **/
std::string GetPureUrl(std::string url);

/**
 * @brief: Get query string
 * @param [in] url: full url with GET parameters
 * @return: query string
 * e.g
 *      [in] /index.html?usr=test&pwd=test
 *      [return] usr=test&pwd=test
 *
 **/
std::string GetQueryStr(std::string url);

/**
 * @brief: Get _GET parameters
 * @param [in] url: full url with parameters
 * @return: _GET parameters
 * e.g
 *      [in] /index.html?usr=testusr&pwd=testpwd
 *          OR
 *          usr=testusr&pwd=testpwd
 *      [return] (<usr, testusr>, <pwd, testpwd>)
 **/
std::map<std::string, std::string> GetParams(std::string url);

/**
 * @brief: Set response:
 *      StatusCode, ReasonPhase, Content-Type, Content-Length,
 *      MsgBody, MsgLen
 * @param [in] fd: file description (file or socket)
 * @param [in] response: http response [status line + parameters + body]
 * @param [in] status: status code
 * @param [in] reason: reason phase
 * @param [in] contentType: Content-Type [e.g. text/html, text/javascript]
 * @param [in] msgBody: http response body
 * @param [in] msgLen: http response body length
 **/
void SetAndWriteBack(int fd, HttpResponse& response,
    std::string status, std::string reason,
    std::string contentType = "text/html", char* msgBody = NULL,
    int msgLen = 0);

/**
 * @brief: Parse IP-string from struct sockaddr_in
 * @param [in] addr: struct sockaddr_in
 * @return: IP-string
 **/
std::string GetIP(struct sockaddr_in* addr);

uint16 GetPort(struct sockaddr_in* addr);

/**
 * @brief: Read HTTP header into buf
 * @param [in] fd: tcp connection
 * @param [out] buf: buffer storing http header and part of body
 * @param [in] bufSize: size of buffer
 * @return: < 0 - error; > 0 - bytes read
 **/
int ReadHTTPHeader(int fd, char* buf, const int bufSize);

/**
 * @brief: Copy $srcLen bytes from $src to $dest
 * @param [out] dest: destination
 * @param [in] src: source
 * @param [in] srcLen: copy bytes
 **/
int Strncpy(char* dest, const char* src, const int srcLen);

/**
 * @brief: convert value to string
 * @param [in] value: integer
 * @param [in] maxLen: max int len
 **/
std::string ToString(int value, int maxLen = 12);

/**
 * @brief: URL decode
 * @param [in] str: url to be decoded
 * @return: url decoded
 * @e.g.
 *  [in]: correctxt?text=utf-8%E4%B8%AD%E6%96%87%E6%B5%8B%E8%AF%95&version=0&ie=UTF-8&callback=callbackfunc
 *  [out]: correctxt?text=utf-8中文测试&version=0&ie=UTF-8&callback=callbackfunc
 **/
std::string URLDecode(std::string str);

#endif
