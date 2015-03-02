#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include <vector>

#include "Common.h"
#include "ConfManager.h"
#include "Thread.h"
#include "SafeQueue.h"
#include "FileTypes.h"
#include "ClientConn.h"

//TODO: Add ConfManager init file parsers
//TODO: Add Base4 decoder (for Chinese characters)

class HttpServer
{
    public:
        HttpServer();

        void RunServer();

    private:
        std::vector<Thread*> _workThreads;

        Thread* _listenThread;

};

#endif
