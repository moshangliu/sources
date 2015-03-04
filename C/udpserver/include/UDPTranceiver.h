#ifndef UDP_TRANCEIVER
#define UDP_TRANCEIVER

#include "Thread.h"
#include "log4cplus/logger.h"

class UDPTranceiver {
private:
    int _port;
    Thread* _listenThread;
public:
    UDPTranceiver(int port);
    ~UDPTranceiver();

    void logger(log4cplus::Logger logger);

    void run();
    void join();
};

#endif