#include "LoggerWrapper.h"
#include "MutexLock.h"

#include <cstdarg>
#include <cstdio>

#include "log4cplus/consoleappender.h"
#include "log4cplus/layout.h"
#include "log4cplus/loggingmacros.h"

using namespace std;
using namespace log4cplus;

LoggerWrapper* LoggerWrapper::_instance = NULL;
pthread_mutex_t LoggerWrapper::_mutex = PTHREAD_MUTEX_INITIALIZER;

LoggerWrapper::LoggerWrapper() {
//    std::auto_ptr<Layout> layout = std::auto_ptr<Layout>(new PatternLayout("%d{%Y-%m-%d %H:%M:%S} %p - %m [%l/pid:%t]%n"));
    std::auto_ptr<Layout> layout = std::auto_ptr<Layout>(new PatternLayout("%d{%Y-%m-%d %H:%M:%S} %p - %m [pid:%t]%n"));

    SharedAppenderPtr appender(new ConsoleAppender());
    appender->setName("myAppenderName");
    appender->setLayout(layout);

    _logger = Logger::getInstance("UDPTranceiver");
    _logger.addAppender(appender);
    _logger.setLogLevel(ALL_LOG_LEVEL);
}

LoggerWrapper* LoggerWrapper::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex);
        if (_instance == NULL) {
            _instance = new LoggerWrapper();
        }
    }

    return _instance;
}

void LoggerWrapper::logger(log4cplus::Logger logger) {
    _logger = logger;
}

Logger LoggerWrapper::logger() {
    return _logger;
}

void LoggerWrapper::debug(const char* format, ...) {
    if (format == NULL) {
        return;
    }

    const int LOG_MAX_LEN = 1024;
    char buf[LOG_MAX_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, LOG_MAX_LEN, format, args);
    va_end(args);

    LOG4CPLUS_DEBUG(_logger, buf);
}

void LoggerWrapper::info(const char* format, ...) {
    if (format == NULL) {
        return;
    }

    const int LOG_MAX_LEN = 1024;
    char buf[LOG_MAX_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, LOG_MAX_LEN, format, args);
    va_end(args);

    LOG4CPLUS_INFO(_logger, buf);
}

void LoggerWrapper::warn(const char* format, ...) {
    if (format == NULL) {
        return;
    }

    const int LOG_MAX_LEN = 1024;
    char buf[LOG_MAX_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, LOG_MAX_LEN, format, args);
    va_end(args);

    LOG4CPLUS_WARN(_logger, buf);
}

void LoggerWrapper::error(const char* format, ...) {
    if (format == NULL) {
        return;
    }

    const int LOG_MAX_LEN = 1024;
    char buf[LOG_MAX_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, LOG_MAX_LEN, format, args);
    va_end(args);

    LOG4CPLUS_ERROR(_logger, buf);
}
