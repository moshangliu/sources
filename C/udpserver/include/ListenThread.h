#ifndef LISTENTHREAD_H_
#define LISTENTHREAD_H_

#include "Common.h"
#include "Thread.h"

class ClientConn;

class ListenThread : public Thread
{
    public:

        ListenThread(uint32 port);

        /**
         * @brief create server sockaddr
         * @param [in] port: service port
         * @return: server sockaddr
         **/
        struct sockaddr_in* GetSvrAddr();

        virtual void* Process();

    private:

        uint32 _port;

        int listenfd;

        void Bind(int listenfd);

        void Accept(int listenfd);

};

#endif
