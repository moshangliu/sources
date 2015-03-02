#ifndef WORKTHREAD_H_
#define WORKTHREAD_H_

#include "Common.h"
#include "ConfManager.h"
#include "Thread.h"
#include "HttpRequest.h"

class ClientConn;

class WorkThread : public Thread
{
    public:

        /**
         * @brief: Init WorkThread with queue, conf
         * @param [in] queue: Http connection file description queue
         * @param [in] conf: Configure manager, shared with HttpServer
         **/
        WorkThread();

        virtual void* Process();

        /**
         * @brief: Process HttpRequest, and write response to fd
         * @param [in] fd:  http connection socket
         * @param [in] request: HttpRequest processed
         * @return: void
         **/
        void HandleReq(uint32 fd, HttpRequest* request);

        /**
         * @brief: Read data from fd, and create HttpRequest object
         *      with it.
         * @param [in] fd: http connection socket
         * @return: HttpRequest object created
         **/
        HttpRequest* CreateHttpRequestFromConn(ClientConn* conn);

        ~WorkThread();

    private:

        /**
         * @brief: Buffer used to store http head
         **/
        char* _buf;
};

#endif
