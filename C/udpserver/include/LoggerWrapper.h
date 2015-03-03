#ifndef LOGGER_WRAPPER_H
#define LOGGER_WRAPPER_H

#include <pthread.h>
#include "log4cplus/logger.h"

class LoggerWrapper {
private:
    log4cplus::Logger _logger;
    static pthread_mutex_t _mutex;
    static LoggerWrapper* _instance;

    LoggerWrapper();

public:
    static LoggerWrapper* Instance();
    void Logger(log4cplus::Logger logger);
    log4cplus::Logger Logger();
};

#endif