#ifndef UDPRECVDATASTRUCTS_H_
#define UDPRECVDATASTRUCTS_H_

#include <string>
#include <map>
#include <tuple>

#include <pthread.h>

#include "UDPFrame.h"

class UDPRecvObj {
private:
    std::string _ip;
    int _port;
    int _packetId;
    byte _frameCount;
    std::map<byte, UDPFrame*> _frames;

    pthread_mutex_t _mutex;

public:
    UDPRecvObj(std::string ip, int port, int packetId, byte frameCount);
    ~UDPRecvObj();

    void put(UDPFrame* frame);
    bool completed();

    std::tuple<byte*, int> assemble();

    std::string ip() { return _ip; }
    int port() { return _port; }
    int packetId() { return _packetId; }
    byte frameCount() { return _frameCount; }
};



#endif /* UDPRECVDATASTRUCTS_H_ */
