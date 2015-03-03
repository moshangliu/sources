#include "LoggerWrapper.h"

#include "log4cplus/consoleappender.h"
#include "log4cplus/layout.h"

using namespace std;
using namespace log4cplus;

LoggerWrapper* LoggerWrapper::_instance = NULL;
pthread_mutex_t LoggerWrapper::_mutex = PTHREAD_MUTEX_INITIALIZER;

LoggerWrapper::LoggerWrapper() {
    std::auto_ptr<Layout> layout = std::auto_ptr<Layout>(new PatternLayout("%d{%Y-%m-%d %H:%M:%S} %p - %m [%l/pid:%t]%n"));

    SharedAppenderPtr appender(new ConsoleAppender());
    appender->setName("myAppenderName");
    appender->setLayout(layout);

    _logger = Logger::getInstance("UDPTranceiver");
    _logger.addAppender(appender);
    _logger.setLogLevel (DEBUG_LOG_LEVEL);
}

LoggerWrapper* LoggerWrapper::Instance() {
    if (_instance == NULL) {
        pthread_mutex_lock(&_mutex);
        if (_instance == NULL) {
            _instance = new LoggerWrapper();
        }
        pthread_mutex_unlock(&_mutex);
    }

    return _instance;
}

void LoggerWrapper::Logger(log4cplus::Logger logger) {
    _logger = logger;
}

Logger LoggerWrapper::Logger() {
    return _logger;
}