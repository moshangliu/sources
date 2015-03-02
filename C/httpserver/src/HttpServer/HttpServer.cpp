#include <iostream>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include "HttpServer.h"
#include "Thread.h"
#include "WorkThread.h"
#include "ListenThread.h"

using namespace std;

HttpServer::HttpServer()
{
    for (uint32 i = 0; i < ConfManager::Instance()->GetWorkThreadCnt(); i++)
    {
        _workThreads.push_back(new WorkThread());
    }
    _listenThread = new ListenThread(ConfManager::Instance()->GetPort());
}

void HttpServer::RunServer()
{
    uint32 workThreadCnt = ConfManager::Instance()->GetWorkThreadCnt();

    _listenThread->Run();
    for (uint32 i = 0; i < workThreadCnt; i++)
    {
        _workThreads[i]->Run();
    }

    _listenThread->Join();
    for (uint32 i = 0; i < workThreadCnt; i++)
    {
        _workThreads[i]->Join();
    }
}

int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);
    HttpServer* server = new HttpServer();
    server->RunServer();
}
