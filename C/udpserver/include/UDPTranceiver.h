#ifndef UDP_TRANCEIVER
#define UDP_TRANCEIVER

#include "Thread.h"

class UDPTranceiver {
private:
    int _port;
    Thread* listenThread;
public:
    UDPTranceiver(int port);
    ~UDPTranceiver();
    void Run();
    void Join();
};

#endif