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

extern const std::string EMPTY;

int Split(std::string str, std::string delim, std::vector<std::string>& parts);

char* CreateCStr(const ulong len);

int readLine(int32 fd, char* buf, uint32 bufSize);

int readn(int32 fd, char* buf, uint32 nbytes);

int writen(int32 fd, const char* buf, uint32 nbytes);

std::string ltrim(std::string str, std::string rmv);

std::string rtrim(std::string str, std::string rmv);

std::string trim(std::string str, std::string rmv);

#endif
