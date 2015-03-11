#ifndef LISTENTHREAD_H_
#define LISTENTHREAD_H_

#include "Common.h"
#include "Thread.h"
#include "UDPFrame.h"
#include "UDPPacketDispatcher.h"
#include "UDPSendDataStructs.h"
#include "UDPRecvDataStructs.h"

class UDPRecvThread : public Thread
{
    public:
        UDPRecvThread(uint32 port, UDPPacketDispatcher* dispatcher,
            UDPAckMap* ackMap, UDPRecvContainer* recvContainer);

        /**
         * @brief create server sockaddr
         * @param [in] port: service port
         * @return: server sockaddr
         **/
        struct sockaddr_in* getSvrAddr();

        virtual void* process();

        int listenFd() { return _listenfd; }

        void stop() {
            _stopFlag = true;
        }

    private:

        uint32 _port;

        int _listenfd;

        UDPPacketDispatcher* _dispatcher;

        UDPAckMap* _ackMap;

        UDPRecvContainer* _recvContainer;

        bool _stopFlag;

        void Bind(int listenfd);

        void accept(int listenfd);

        void sendAck(UDPFrame* frame, struct sockaddr_in* clientAddr);

};

#endif
