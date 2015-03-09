#ifndef UDPRECVDATASTRUCTS_H_
#define UDPRECVDATASTRUCTS_H_

#include <string>
#include <map>
#include <tuple>

#include <pthread.h>

#include "Common.h"
#include "UDPFrame.h"
#include "SafeMap.h"
#include "SafePriorityQueue.h"
#include "UDPRetryTimeSpan.h"

class UDPRecvObj {
private:
    std::string _ip;
    int _port;
    int _packetId;
    byte _frameCount;
    std::map<byte, UDPFrame*> _frames;

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

class UDPRecvObjMeta {
private:
    std::string _ip;
    int _port;
    int _packetId;

    long _expireTsUs;

public:
    UDPRecvObjMeta(std::string ip, int port, int packetId)
        : _ip(ip), _port(port), _packetId(packetId){
        _expireTsUs = current_us() + UDPRetryTimeSpan::instance()->allWaitTimeUs() * 2;
    }

    std::string ip() { return _ip; }
    int port() { return _port; }
    int packetId() { return _packetId; }
    long expireTsUs() { return _expireTsUs; }

    std::string packetKey() {
        return makeRecvPacketKey(_ip, _port, _packetId);
    }
};

class UDPRecvObjMeta4MinHeap {
public:
    bool operator() (UDPRecvObjMeta* v1, UDPRecvObjMeta* v2) {
        return (v1->expireTsUs()) > (v2->expireTsUs());
    }
};

class UDPRecvContainer {
private:
    std::map<std::string, UDPRecvObj*> _udpRecvObjs;
    std::priority_queue<UDPRecvObjMeta*> _udpRecvMetas;

    static pthread_mutex_t _mutex4instance;
    static UDPRecvContainer* _instance;

    pthread_mutex_t _mutex4container;
    UDPRecvContainer();

public:
    static UDPRecvContainer* instance();

    /**
     * @return: <completed, content, contentLen>
     **/
    std::tuple<bool, byte*, int> putOrAssemble(std::string ip, int port, UDPFrame* frame);

    int expireOrReturnSleepUs();
};


#endif /* UDPRECVDATASTRUCTS_H_ */
