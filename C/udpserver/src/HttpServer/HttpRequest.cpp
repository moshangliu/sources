#include "HttpRequest.h"
#include <string>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

HttpRequest::HttpRequest()
{
    _postData = NULL;
    _postLen  = 0;
}

HttpRequest* HttpRequest::CreateHttpRequest(const char* data, uint32 len)
{
    if (data == NULL)
    {
        return NULL;
    }

    const char* t_data = data;
    HttpRequest* request = new HttpRequest();

    data = CutHttpHead(data, request);
    if (data == NULL)
    {
        return NULL;
    }

    data = CutHttpParams(data, request);
    if (data == NULL)
    {
        return NULL;
    }

    ulong headLen = (ulong)data - ulong(t_data);

    char* postData = NULL;

    if (len > headLen)
    {
        postData = CreateCStr(len - headLen + 1);
        Strncpy(postData, data, len - headLen);
    }

    request->_postData = postData;
    request->_postLen = len - headLen;

    return request;
}

const char* HttpRequest::CutHttpHead(const char* data, HttpRequest* request)
{
    if (data == NULL || request == NULL)
    {
        return NULL;
    }

    const char* t_data = strstr(data, CRLF);
    if (t_data == NULL)
    {
        return NULL;
    }

    ulong len = (ulong)t_data - (ulong)data;
    char* headLine = CreateCStr(len + 1);
    strncpy(headLine, data, len);
    vector<string> vec;
    Split(string(headLine), SP, vec);
    delete [] headLine;

    if (vec.size() != 3)
    {
        return NULL;
    }

    HTTP_METHOD method = GetHttpMethod(vec[0]);
    if (method == OTHER)
    {
        return NULL;
    }
    request->_method = method;
    request->_url = GetPureUrl(vec[1]);
    request->_fileExt = GetFileExtension(request->_url);
    request->_urlAll = vec[1];
    request->_getStr = GetQueryStr(vec[1]);
    request->_get = GetParams(vec[1]);
    request->_version = vec[2];

    return t_data + strlen(CRLF);
}

const char* HttpRequest::CutHttpParams(const char* data, HttpRequest* request)
{
    if (data == NULL || request == NULL)
    {
        return NULL;
    }

    if (strstr(data, CRLF) == data)
    {
        return data + strlen(CRLF);
    }

    const char* t_data = strstr(data, CRLFCRLF);

    if (t_data == NULL)
    {
        return NULL;
    }

    ulong len = (ulong)t_data - (ulong)data;
    char* paramStr = CreateCStr(len + 1);
    strncpy(paramStr, data, len);
    vector<string> paramLines;
    Split(string(paramStr), CRLF, paramLines);
    delete [] paramStr;

    for (vector<string>::iterator it = paramLines.begin();
        it != paramLines.end(); it++)
    {
        string str = *it;
        if (str.size() <= 0)
        {
            continue;
        }
        int32 idx = str.find(SPLIT);
        if (idx > 0)
        {
            string key = trim(str.substr(0, idx), CRLF);
            string value = trim(str.substr(idx + 2, str.size() - (idx + 2)), CRLF);
            request->_headParams[key] = value;
        }
    }

    return t_data + strlen(CRLFCRLF);
}

HttpRequest::~HttpRequest()
{
    delete [] _postData;
}

string HttpRequest::ToString() const
{
    string res;
    res += "Method=[";
    res += HttpMethodToString(_method) + "], ";
    res += "Url=[";
    res += _url + "], ";
    res += "Version=[";
    res += _version + "] " + CRLF;

    for (map<string, string>::const_iterator it = _headParams.begin();
        it != _headParams.end(); it++)
    {
        res += it->first + SPLIT;
        res += it->second + CRLF;
    }

    res += CRLF;
    res += "GET-PARAMS: ";
    for (map<string, string>::const_iterator it = _get.begin();
        it != _get.end(); it++)
    {
        res += "[" + it->first + "=" + it->second + "]";
    }

    res += CRLF;
    res += "POST-PARAMS: ";
    for (map<string, string>::const_iterator it = _post.begin();
        it != _post.end(); it++)
    {
        res += "[" + it->first + "=" + it->second + "]";
    }
    return res;
}
