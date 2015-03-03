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
    /*
    std::auto_ptr<Layout> layout = std::auto_ptr<Layout>(new PatternLayout("%d{%Y-%m-%d %H:%M:%S} %p - %m [%l/pid:%t]%n"));

    SharedAppenderPtr appender(new ConsoleAppender());
    appender->setName("myAppenderName");
    appender->setLayout(layout);

    _logger = Logger::getInstance("UDPTranceiver");
    _logger.addAppender(appender);
    _logger.setLogLevel (DEBUG_LOG_LEVEL);
*/
    listenThread = new ListenThread(port);
}

UDPTranceiver::~UDPTranceiver() {
    delete listenThread;
}

void UDPTranceiver::SetLogger(log4cplus::Logger logger) {
    LoggerWrapper::Instance()->Logger(logger);
}

void UDPTranceiver::Run() {
    LOG4CPLUS_DEBUG(LoggerWrapper::Instance()->Logger(), "LOG4CPLUS, UDPTranceiver started");
    listenThread->Run();
}

void UDPTranceiver::Join() {
    listenThread->Join();
}