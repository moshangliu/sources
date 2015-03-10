#include "Common.h"
#include "UDPTranceiver.h"
#include "UDPRecvThread.h"
#include "UDPSendThread.h"
#include "UDPResendThread.h"
#include "UDPExpireThread.h"
#include "LoggerWrapper.h"
#include "UDPFrameHelper.h"
#include "UDPSendDataStructs.h"

#include <string>
#include <iostream>

#include "log4cplus/loggingmacros.h"

using namespace std;
using namespace log4cplus;

UDPTranceiver::UDPTranceiver(int port) : _port(port) {
    _udpRecvThread = new UDPRecvThread(port);

    int listenFd = ((UDPRecvThread*)_udpRecvThread)->listenFd();
    _udpSendThread = new UDPSendThread(listenFd);
    _udpResendThread = new UDPResendThread(listenFd);
    _udpExpireThread = new UDPExpireThread();

}

UDPTranceiver::~UDPTranceiver() {
    delete _udpRecvThread;
    delete _udpSendThread;
    delete _udpResendThread;
    delete _udpExpireThread;
}

void UDPTranceiver::logger(log4cplus::Logger logger) {
    LoggerWrapper::instance()->logger(logger);
}

void UDPTranceiver::run() {
    LoggerWrapper::instance()->info("UDPTranceiver started on port:%d", _port);
    _udpRecvThread->run();
    _udpSendThread->run();
    _udpResendThread->run();
    _udpExpireThread->run();
}

void UDPTranceiver::join() {
    _udpRecvThread->join();
    _udpSendThread->join();
    _udpResendThread->join();
    _udpExpireThread->join();
}

void UDPTranceiver::send(std::string ip, int port, char* data, int dataLen) {
    if (data == NULL || dataLen <= 0) {
        LoggerWrapper::instance()->warn("UDPTrace, SEND_PACKET, %s:%d, BODY_EMPTY", ip.c_str(), port);
        return;
    }

    vector<UDPFrame*>* frames = UDPFrameHelper::segment((byte*)data, dataLen);
    for (vector<UDPFrame*>::iterator it = frames->begin(); it != frames->end(); it++) {
        UDPSendQueue::instance()->push(new UDPResendObj(ip, port, *it));
    }

    delete frames;
}
