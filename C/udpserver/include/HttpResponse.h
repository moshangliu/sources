#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

#include "Common.h"
#include <map>
#include <string>


class HttpResponse
{
    public:
        HttpResponse();

        std::string GetHttpVersion() const;

        std::string GetStatusCode() const;

        std::string GetReasonPhase() const;

        std::map<std::string, std::string> GetParams() const;

        const char* GetMsgBody() const;

        uint32 GetMsgLen() const;

        void SetHttpVersion(const std::string httpVersion);

        void SetStatusCode(const std::string statusCode);

        void SetReasonPhase(const std::string reasonPhase);

        void SetStatusAndReason(const std::string statusCode,
            const std::string reasonPhase);

        void SetHeaderParam(const std::string key, const std::string value);

        void SetBodyAndLen(const char* body, uint32 len);

        std::string GetResponseHead();

        std::string GetResponseStatusLine();

        ~HttpResponse();

    private:
        std::string _httpVersion;

        std::string _statusCode;

        std::string _reasonPhase;

        std::map<std::string, std::string> _params;

        char* _msgBody;

        uint32 _msgLen;
};

#endif
