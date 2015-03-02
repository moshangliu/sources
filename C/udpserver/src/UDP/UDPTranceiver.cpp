#include "Common.h"
#include "UDPTranceiver.h"
#include "ListenThread.h"

#include <string>
#include <iostream>

#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/layout.h"
#include <log4cplus/configurator.h>

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

UDPTranceiver::UDPTranceiver(int port) : _port(port) {
    listenThread = new ListenThread(port);

    std::auto_ptr<Layout> layout = std::auto_ptr<Layout>(new PatternLayout("%d{%Y-%m-%d %H:%M:%S} %p - %m [%l/pid:%t]%n"));

    SharedAppenderPtr appender(new ConsoleAppender());
    appender->setName("myAppenderName");
    appender->setLayout(layout);

    _logger = Logger::getInstance("UDPTranceiver");
    _logger.addAppender(appender);
    _logger.setLogLevel (DEBUG_LOG_LEVEL);
}

UDPTranceiver::~UDPTranceiver() {
    delete listenThread;
}

void UDPTranceiver::SetLogger(Logger logger) {
    _logger = logger;
}

void UDPTranceiver::Run() {
    LOG4CPLUS_DEBUG(_logger, "LOG4CPLUS, UDPTranceiver started");
    listenThread->Run();
}

void UDPTranceiver::Join() {
    listenThread->Join();
}