#include "HttpResponse.h"
#include "stdio.h"
#include "string.h"

using namespace std;

HttpResponse::HttpResponse()
{
    _msgBody = NULL;
    _msgLen = 0;
}

string HttpResponse::GetHttpVersion() const
{
    return _httpVersion;
}

string HttpResponse::GetStatusCode() const
{
    return _statusCode;
}

string HttpResponse::GetReasonPhase() const
{
    return _reasonPhase;
}

map<string, string> HttpResponse::GetParams() const
{
    return _params;
}

const char* HttpResponse::GetMsgBody() const
{
    return _msgBody;
}

uint32 HttpResponse::GetMsgLen() const
{
    return _msgLen;
}

void HttpResponse::SetHttpVersion(const string httpVersion)
{
    _httpVersion = httpVersion;
}

void HttpResponse::SetStatusCode(const string statusCode)
{
    _statusCode = statusCode;
}

void HttpResponse::SetReasonPhase(const std::string reasonPhase)
{
    _reasonPhase = reasonPhase;
}

void HttpResponse::SetStatusAndReason(const std::string statusCode,
    const std::string reasonPhase)
{
    this->SetStatusCode(statusCode);
    this->SetReasonPhase(reasonPhase);
}

void HttpResponse::SetHeaderParam(const std::string key,
    const std::string value)
{
    _params[key] = value;
}

void HttpResponse::SetBodyAndLen(const char* body, uint32 len)
{
    delete [] _msgBody;
    if (body == NULL)
    {
        _msgBody = NULL;
        _msgLen = 0;
        return;
    }

    _msgBody = new char[len];
    strncpy(_msgBody, body, len);
    _msgLen = len;
}

string HttpResponse::GetResponseHead()
{
    string head;
    head += _httpVersion + SP + _statusCode + SP + _reasonPhase + CRLF;

    for (map<string, string>::const_iterator it = _params.begin();
        it != _params.end(); it++)
    {
        head += it->first + SPLIT + it->second + CRLF;
    }

    head += CRLF;

    return head;
}

string HttpResponse::GetResponseStatusLine()
{
    string head;
    head += _httpVersion + SP + _statusCode + SP + _reasonPhase + CRLF;

    return head;
}

HttpResponse::~HttpResponse()
{
    delete [] _msgBody;
}
