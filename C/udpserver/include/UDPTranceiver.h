#ifndef UDP_TRANCEIVER
#define UDP_TRANCEIVER

#include "Thread.h"
#include "log4cplus/logger.h"

class UDPTranceiver {
private:
    int _port;
    Thread* _udpRecvThread;
    Thread* _udpSendThread;
    Thread* _udpResendThread;
    Thread* _udpExpireThread;
public:
    UDPTranceiver(int port);
    ~UDPTranceiver();

    void logger(log4cplus::Logger logger);

    void run();
    void join();

    void send(std::string ip, int port, char* data, int dataLen);
};

#endif
