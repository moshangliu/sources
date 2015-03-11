#ifndef LISTENTHREAD_H_
#define LISTENTHREAD_H_

#include "Common.h"
#include "Thread.h"
#include "UDPFrame.h"
#include "UDPPacketDispatcher.h"
#include "UDPSendDataStructs.h"
#include "UDPRecvDataStructs.h"
#include "UDPPacketSendSuccessHandler.h"

class UDPRecvThread : public Thread
{
    public:
        UDPRecvThread(uint32 port, UDPPacketDispatcher* dispatcher,
            UDPAckMap* ackMap, UDPRecvContainer* recvContainer,
            UDPPacketMap* packetMap, UDPPacketSendSuccessHandler* successHandler);

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

        void setSuccessHandler(UDPPacketSendSuccessHandler* successHandler) {
            _successHandler = successHandler;
        }

    private:

        uint32 _port;

        int _listenfd;

        UDPPacketDispatcher* _dispatcher;

        UDPAckMap* _ackMap;

        UDPRecvContainer* _recvContainer;

        UDPPacketMap* _packetMap;

        UDPPacketSendSuccessHandler* _successHandler;

        bool _stopFlag;

        void Bind(int listenfd);

        void accept(int listenfd);

        void sendAck(UDPFrame* frame, struct sockaddr_in* clientAddr);

};

#endif
