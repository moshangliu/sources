#ifndef UDP_TRANCEIVER
#define UDP_TRANCEIVER

#include "Thread.h"
#include "log4cplus/logger.h"

class UDPTranceiver {
private:
    int _port;
    Thread* listenThread;
    log4cplus::Logger _logger;
public:
    UDPTranceiver(int port);
    ~UDPTranceiver();

    void SetLogger(log4cplus::Logger logger);

    void Run();
    void Join();
};

#endif