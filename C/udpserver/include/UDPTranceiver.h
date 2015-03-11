#ifndef UDP_TRANCEIVER
#define UDP_TRANCEIVER

#include "Common.h"
#include "Thread.h"
#include "UDPPacketDispatcher.h"
#include "UDPSendDataStructs.h"
#include "UDPRecvDataStructs.h"
#include "log4cplus/logger.h"
#include "UDPRecvThread.h"
#include "UDPExpireThread.h"
#include "UDPSendThread.h"
#include "UDPResendThread.h"

#include <signal.h>

class UDPTranceiver {
private:
    int _port;
    UDPRecvThread* _udpRecvThread;
    UDPSendThread* _udpSendThread;
    UDPResendThread* _udpResendThread;
    UDPExpireThread* _udpExpireThread;

    UDPPacketDispatcher* _dispatcher;

    UDPSendQueue* _sendQueue;
    UDPResendQueue* _resendQueue;
    UDPAckMap* _ackMap;
    UDPRecvContainer* _recvContainer;
public:
    UDPTranceiver(int port);
    ~UDPTranceiver();

    void logger(log4cplus::Logger logger);

    void run();
    void join();

    void send(byte type, std::string ip, int port, char* data, int dataLen);

    void registerHandler(byte type, UDPPacketReceivedHandler* handler) {
        _dispatcher->registerHandler(type, handler);
    }
    void unregisterHandler(byte type) {
        _dispatcher->unregisterHandler(type);
    }
    void setDefaultHandler(UDPPacketReceivedHandler* handler) {
        _dispatcher->setDefaultHandler(handler);
    }

    void stop() {
//        pthread_kill(_udpRecvThread->threadId(), SIGKILL);
//        pthread_kill(_udpSendThread->threadId(), SIGKILL);
//        pthread_kill(_udpResendThread->threadId(), SIGKILL);
//        pthread_kill(_udpExpireThread->threadId(), SIGKILL);
        _udpRecvThread->stop();
        _udpSendThread->stop();
        _udpResendThread->stop();
        _udpExpireThread->stop();
    }
};

#endif
