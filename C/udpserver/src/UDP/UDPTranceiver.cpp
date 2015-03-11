#include "Common.h"
#include "UDPTranceiver.h"
#include "UDPRecvThread.h"
#include "UDPSendThread.h"
#include "UDPResendThread.h"
#include "UDPExpireThread.h"
#include "LoggerWrapper.h"
#include "UDPFrameHelper.h"
#include "UDPSendDataStructs.h"
#include "boost/shared_array.hpp"
#include "boost/shared_ptr.hpp"

#include <cstring>
#include <string>
#include <iostream>

#include "log4cplus/loggingmacros.h"

using namespace std;
using namespace boost;
using namespace log4cplus;

UDPTranceiver::UDPTranceiver(int port) : _port(port) {
    _dispatcher = new UDPPacketDispatcher();
    _sendQueue = new UDPSendQueue();
    _resendQueue = new UDPResendQueue();
    _ackMap = new UDPAckMap();
    _recvContainer = new UDPRecvContainer();

    _udpRecvThread = new UDPRecvThread(port, _dispatcher, _ackMap, _recvContainer);
    int listenFd = ((UDPRecvThread*)_udpRecvThread)->listenFd();

    _udpSendThread = new UDPSendThread(listenFd, _sendQueue, _resendQueue, _ackMap);
    _udpResendThread = new UDPResendThread(listenFd, _resendQueue, _ackMap);
    _udpExpireThread = new UDPExpireThread(_recvContainer);
}

UDPTranceiver::~UDPTranceiver() {
    delete _udpRecvThread;
    delete _udpSendThread;
    delete _udpResendThread;
    delete _udpExpireThread;

    delete _sendQueue;
    delete _resendQueue;
    delete _ackMap;
    delete _recvContainer;
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

void UDPTranceiver::send(byte type, std::string ip, int port, char* data, int dataLen) {
    if (data == NULL || dataLen <= 0) {
        LoggerWrapper::instance()->warn("UDPTrace, SEND_PACKET, %s:%d, BODY_EMPTY", ip.c_str(), port);
        return;
    }

    shared_array<char> dataNew(new char[dataLen + 1]);
    dataNew.get()[0] = type;
    memcpy(dataNew.get() + 1, data, dataLen);

    boost::shared_ptr<vector<UDPFrame*>> frames(UDPFrameHelper::segment((byte*)dataNew.get(), dataLen + 1));
    for (vector<UDPFrame*>::iterator it = frames.get()->begin(); it != frames.get()->end(); it++) {
        _sendQueue->push(new UDPResendObj(ip, port, *it));
    }
}
