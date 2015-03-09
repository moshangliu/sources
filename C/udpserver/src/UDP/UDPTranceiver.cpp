#include "Common.h"
#include "UDPTranceiver.h"
#include "ListenThread.h"
#include "LoggerWrapper.h"
#include "UDPFrameHelper.h"
#include "UDPSendDataStructs.h"

#include <string>
#include <iostream>

#include "log4cplus/loggingmacros.h"

using namespace std;
using namespace log4cplus;

UDPTranceiver::UDPTranceiver(int port) : _port(port) {
    _listenThread = new ListenThread(port);
}

UDPTranceiver::~UDPTranceiver() {
    delete _listenThread;
}

void UDPTranceiver::logger(log4cplus::Logger logger) {
    LoggerWrapper::instance()->logger(logger);
}

void UDPTranceiver::run() {
    LoggerWrapper::instance()->info("UDPTranceiver started on port:%d", _port);
    _listenThread->run();
}

void UDPTranceiver::join() {
    _listenThread->join();
}

void UDPTranceiver::send(std::string ip, int port, char* data, int dataLen) {
    if (data == NULL || dataLen <= 0) {
        LoggerWrapper::instance()->warn("UDPTrace, SEND_PACKET, %s:%d, BODY_EMPTY", ip.c_str(), port);
        return;
    }

    vector<UDPFrame*>* frames = UDPFrameHelper::segment((byte*)data, dataLen);
    for (vector<UDPFrame*>::iterator it = frames->begin(); it != frames->end(); it++) {
        UDPSendQueue::instance()->push(new UDPResendObj(*it));

        int packetId = (*it)->packetId();
        byte frameCount = (*it)->frameCount();
        byte frameIndex = (*it)->frameIndex();
        UDPAckMap::instance()->setAcked(packetId, frameIndex, false);

        LoggerWrapper::instance()->debug("UDPTrace, SEND_PACKET, %s:%d, PACKET_ID:%d, %d/%d",
            ip.c_str(), port, packetId, frameIndex, frameCount);
    }

    delete frames;
}
