#include "Common.h"

#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

const std::string EMPTY = "";
const string WHITE_SPACES = " \r\n";

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
