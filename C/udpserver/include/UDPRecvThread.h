#ifndef LISTENTHREAD_H_
#define LISTENTHREAD_H_

#include "Common.h"
#include "Thread.h"
#include "UDPFrame.h"

class UDPRecvThread : public Thread
{
    public:
        UDPRecvThread(uint32 port);

        /**
         * @brief create server sockaddr
         * @param [in] port: service port
         * @return: server sockaddr
         **/
        struct sockaddr_in* getSvrAddr();

        virtual void* process();

        int listenFd() { return _listenfd; }

    private:

        uint32 _port;

        int _listenfd;

        void Bind(int listenfd);

        void accept(int listenfd);

        void sendAck(UDPFrame* frame, struct sockaddr_in* clientAddr);

};

#endif
