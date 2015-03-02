#include "Common.h"

#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

const std::string EMPTY = "";



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

