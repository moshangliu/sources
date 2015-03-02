#include "Common.h"
#include "HttpResponse.h"

#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

const std::string HTTP_1_0 = "HTTP/1.0";
const std::string HTTP_1_1 = "HTTP/1.1";
const std::string EMPTY = "";
const std::string STREAM_TYPE = "application/octet-stream";

const char* SP = " ";
const char* CRLF = "\r\n";
const char* CRLFCRLF = "\r\n\r\n";
const char* SPLIT = ": ";
const char* DOT = ".";

int readn(int32 fd, char* buf, uint32 nbytes)
{
    if (fd < 0 || buf == NULL)
    {
        return -1;
    }

    int nleft = nbytes;
    while (nleft > 0)
    {
        int nread = 0;
        if ((nread = read(fd, buf, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -2;
        }

        if (nread == 0)
        {
            break;
        }

        nleft -= nread;
        buf += nread;
    }

    return nbytes - nleft;
}

int writen(int32 fd, const char* buf, uint32 nbytes)
{
    if (fd < 0 || buf == NULL)
    {
        return -1;
    }

    int nleft = nbytes;
    while (nleft > 0)
    {
        int nwrite = 0;
        if ((nwrite = write(fd, buf, nleft)) < 0)
        {
            return -2;
        }

        nleft -= nwrite;
        buf += nwrite;
    }

    return nbytes;
}

// TODO: Add UT cases
int readLine(int32 fd, char* buf, uint32 bufSize)
{
    if (fd < 0 || buf == NULL || bufSize == 0)
    {
        return -1;
    }

    uint32 nread = 0;
    while (bufSize > nread)
    {
        if (readn(fd, buf + nread, 1) == 0)
        {
            break;
        }

        if (buf[nread] == '\n')
        {
            nread++;
            break;
        }

        nread++;
    }
    return nread;
}

int Split(string str, string delim, vector<string>& parts)
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

char* CreateCStr(const ulong len)
{
    if (len == 0)
    {
        return NULL;
    }

    char* c_str = new char[len];
    memset(c_str, 0, len);
    return c_str;
}

HTTP_METHOD GetHttpMethod(std::string method)
{
    if (method == "GET")
    {
        return GET;
    }
    if (method == "POST")
    {
        return POST;
    }
    if (method == "HEAD")
    {
        return HEAD;
    }
    if (method == "PUT")
    {
        return PUT;
    }
    if (method == "DELETE")
    {
        return DELETE;
    }
    if (method == "TRACE")
    {
        return TRACE;
    }
    if (method == "CONNECT")
    {
        return CONNECT;
    }
    if (method == "OPTIONS")
    {
        return OPTIONS;
    }
    return OTHER;
}

string HttpMethodToString(HTTP_METHOD method)
{
    if (method == GET)
    {
        return "GET";
    }
    if (method == POST)
    {
        return "POST";
    }
    if (method == HEAD)
    {
        return "HEAD";
    }
    if (method == PUT)
    {
        return "PUT";
    }
    if (method == DELETE)
    {
        return "DELETE";
    }
    if (method == TRACE)
    {
        return "TRACE";
    }
    if (method == CONNECT)
    {
        return "CONNECT";
    }
    if (method == OPTIONS)
    {
        return "OPTIONS";
    }
    return "OTHER";
}

int WriteResponse(int fd, HttpResponse& response)
{
    if (fd < 0)
    {
        return -1;
    }

    string head = response.GetResponseHead();

    writen(fd, head.c_str(), head.size());
    writen(fd, response.GetMsgBody(), response.GetMsgLen());
    return 0;
}

string IntToStr(int num)
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

string GetFileExtension(string path)
{
    size_t idx = path.rfind(DOT);
    if (idx == string::npos || idx == path.size() - 1)
    {
        return EMPTY;
    }

    return path.substr(idx + 1, path.size() - (idx + 1));
}

string GetPureUrl(string url)
{
    size_t idx = url.find("?");
    return idx == string::npos
        ? url
        : url.substr(0, idx);
}

std::string GetQueryStr(std::string url)
{
    size_t idx = url.find("?");
    return idx == string::npos
        ? ""
        : url.substr(idx + 1, url.size() - (idx + 1));
}

map<string, string> GetParams(std::string url)
{
    map<string, string> params;
    size_t idx = url.find("?");
    // if (idx != string::npos)
    {
        string paramsStr = url.substr(idx + 1, url.size() - idx - 1);
        vector<string> parts;
        Split(paramsStr, "&", parts);
        for (vector<string>::iterator it = parts.begin();
            it != parts.end(); it++)
        {
            vector<string> t_kv;
            Split(*it, "=", t_kv);
            if (t_kv.size() == 2)
            {
                params[URLDecode(t_kv[0])] = URLDecode(t_kv[1]);
                // cout << URLDecode(t_kv[0]) << "|" << URLDecode(t_kv[1]) << endl;
            }
        }
    }
    return params;
}

void SetAndWriteBack(int fd, HttpResponse& response,
    std::string status, std::string reason,
    std::string contentType, char* msgBody,
    int msgLen)
{
    if (fd < 0 || msgLen < 0)
    {
        return;
    }

    response.SetStatusAndReason(status, reason);
    response.SetHeaderParam("Content-Length", IntToStr(msgLen));
    response.SetHeaderParam("Content-Type", contentType);
    response.SetBodyAndLen(msgBody, msgLen);

    WriteResponse(fd, response);
}

string GetIP(struct sockaddr_in* addr)
{
    const int IP_STR_LEN = 32;
    char ipStr[IP_STR_LEN];
    memset(ipStr, 0, IP_STR_LEN);

    return inet_ntop(AF_INET, (void*)(&(addr->sin_addr)), ipStr, IP_STR_LEN) != NULL
        ? string(ipStr)
        : EMPTY;

}

uint16 GetPort(struct sockaddr_in* addr)
{
    return ntohs(addr->sin_port);
}

int ReadHTTPHeader(int fd, char* buf, const int bufSize)
{
    if (fd < 0 || buf == NULL || bufSize <= 0)
    {
        return -1;
    }

    memset(buf, 0, bufSize);
    int nread = 0;

    while (nread < bufSize)
    {
        int n = read(fd, buf + nread, bufSize - nread);
        if (n < 0)
        {
            cerr << "Failed to read data from socket [" << fd << "]" << endl;
            return -2;
        }

        nread += n;
        if (strstr(buf, CRLFCRLF) != NULL)
        {
            break;
        }

        if (n == 0)
        {
            cerr << "Header is not complete"  << endl;
            return -3;
        }
    }

    if (strstr(buf, CRLFCRLF) == NULL)
    {
        cerr << "Header size exceeds max-value [" << bufSize << "]" << endl;
        return -4;
    }
    return nread;
}

int Strncpy(char* dest, const char* src, const int srcLen)
{
    if (dest == NULL || src == NULL || srcLen < 0)
    {
        return -1;
    }

    for (int i = 0; i < srcLen; i++)
    {
        dest[i] = src[i];
    }
    return 0;
}

string ToString(int value, int maxLen)
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

string URLDecode(string str)
{
    map<char, byte> KV;
    KV['0'] = (byte)0x0;
    KV['1'] = (byte)0x1;
    KV['2'] = (byte)0x2;
    KV['3'] = (byte)0x3;
    KV['4'] = (byte)0x4;
    KV['5'] = (byte)0x5;
    KV['6'] = (byte)0x6;
    KV['7'] = (byte)0x7;
    KV['8'] = (byte)0x8;
    KV['9'] = (byte)0x9;
    KV['A'] = (byte)0xA;
    KV['B'] = (byte)0xB;
    KV['C'] = (byte)0xC;
    KV['D'] = (byte)0xD;
    KV['E'] = (byte)0xE;
    KV['F'] = (byte)0xF;
    KV['a'] = (byte)0xA;
    KV['b'] = (byte)0xB;
    KV['c'] = (byte)0xC;
    KV['d'] = (byte)0xD;
    KV['e'] = (byte)0xE;
    KV['f'] = (byte)0xF;

    string res;
    int idx = 0;
    while ((idx = str.find("%")) != string::npos
        && idx + 2 < str.size())
    {
        res += str.substr(0, idx);
        char c = (KV[str[idx + 1]] << 4) + KV[str[idx + 2]];
        res += c;
        str = str.substr(idx + 3, string::npos);
    }

    res += str;
    return res;
}
