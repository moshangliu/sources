#include <iostream>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "WorkThread.h"
#include "HttpResponse.h"
#include "FileTypes.h"
#include "SafeQueueInstance.h"
#include "ClientConn.h"

using namespace std;

const uint32 BUF_SIZE = 2048;

WorkThread::WorkThread()
{
    _buf = new char[BUF_SIZE];
}

void* WorkThread::Process()
{
    while (true)
    {
        ClientConn* conn = SafeQueueInstance::Instance()->Pop();
        uint32 connfd = conn->GetConnFd();

        HttpRequest* request = CreateHttpRequestFromConn(conn);
        if (request == NULL)
        {
            close(connfd);
            delete conn;
            continue;
        }

        HandleReq(connfd, request);

        delete request;
        delete conn;
        close(connfd);
    }

    return NULL;
}

void WorkThread::HandleReq(uint32 fd, HttpRequest* request)
{
    if (request == NULL)
    {
        return;
    }

    // Add process logic
    string body = request->ToString();
    cout << "Sent: " << body << endl;

    HttpResponse response;
    response.SetHttpVersion(request->GetVersion());
    response.SetStatusAndReason("200", "OK");
    response.SetHeaderParam("Content-Length", ToString(body.size()));
    response.SetBodyAndLen(body.c_str(), body.size());

    WriteResponse(fd, response);
}

HttpRequest* WorkThread::CreateHttpRequestFromConn(ClientConn* conn)
{
    uint32 fd = conn->GetConnFd();
    memset(_buf, 0, BUF_SIZE);

    int nread = 0;
    if ((nread = ReadHTTPHeader(fd, _buf, BUF_SIZE)) < 0)
    {
        return NULL;
    }

    HttpRequest* request = HttpRequest::CreateHttpRequest(_buf, nread);
    if (request == NULL)
    {
        return NULL;
    }
    request->SetClientIP(conn->GetIP());
    request->SetClientPort(conn->GetPort());

    map<string, string> params = request->GetHeadParams();
    if (params.find("Content-Length") == params.end())
    {
        // TODO: Will check if "Content-Length" is necessary
        // for HEAD, CONNECT an so on, and add them.
        if (request->GetMethod() == GET)
        {
            return request;
        }
        HttpResponse response;
        response.SetHttpVersion(request->GetVersion());
        response.SetStatusAndReason("411", "Length Required");
        response.SetHeaderParam("Content-Length", "0");
        WriteResponse(fd, response);

        return NULL;
    }

    int postAllLen = atoi(params["Content-Length"].c_str());
    if (postAllLen == 0)
    {
        return request;
    }

    uint32 postReadLen = request->GetPostDataLen();
    const char* postDataRead = request->GetPostData();
    int postLeftLen = postAllLen - postReadLen;

    if (postLeftLen > 0)
    {
        // Make postdata end with '\0', be easy to parse post params
        char* postData = CreateCStr(postAllLen + 1);
        if (postReadLen > 0)
        {
            strncpy(postData, postDataRead, postReadLen);
            delete [] postDataRead;
        }

        // When Content-Length is bigger than real post body length,
        // it will cause WorkThread forever-wait until client close connection
        readn(fd, postData + postReadLen, postLeftLen);
        request->SetPostData(postData);
        request->SetPostDataLen(postAllLen);
    }

    map<string, string> post = GetParams(string(request->GetPostData()));
    request->SetPOST(post);

    return request;
}

WorkThread::~WorkThread()
{
    delete [] _buf;
}
