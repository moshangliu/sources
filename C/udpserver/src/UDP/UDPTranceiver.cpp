#include "Common.h"
#include "UDPTranceiver.h"
#include "ListenThread.h"
#include "LoggerWrapper.h"

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
//    LOG4CPLUS_DEBUG(LoggerWrapper::instance()->logger(), "UDPTranceiver started");
    LoggerWrapper::instance()->debug("UDPTranceiver started on port:%d", _port);
    _listenThread->run();
}

void UDPTranceiver::join() {
    _listenThread->join();
}
