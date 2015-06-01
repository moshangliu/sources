#ifndef FASTCGI_H_
#define FASTCGI_H_

#include "Common.h"
#include "FastCGIFormat.h"
#include "HttpRequest.h"

class FastCGI
{
    public:
        FastCGI();

        virtual ~FastCGI();

        int GetFastCGIFd();

        int ConnectFastCGI(const char *IP, const uint32 port);

        void SetRequestID(const int requestID);

        void AddParam(const char *name, const char *value);

        int Process(const int sockfd, const HttpRequest* req);

    private:
        int SendHeader(uchar type, int contentLength, int paddingLength);

        int BeginRequest();

        int SendParam();

        int SendPost(const HttpRequest* req);

        void SetStatus(HttpResponse* response);

        int Response(const int sockfd, const HttpRequest* req);

        FCGI_Header *_header;

        HttpResponse *_response;

        char *_buf;

        uint32 _paramsLen;

        int _requestID;

        int _FastCGIfd;
};

#endif /* FASTCGI_H_ */
