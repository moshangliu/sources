#ifndef UDPRESENDQUEUE_H_
#define UDPRESENDQUEUE_H_

#include <vector>
#include <string>
#include "UDPFrame.h"
#include "SafePriorityQueue.h"
#include "SafeQueue.h"
#include "SafeMap.h"

class UDPResendObj {
private:
    std::string _ip;
    int _port;
    UDPFrame* _frame;

    int _triedCnt;
    long _sendTsUs;

public:
    UDPResendObj(std::string ip, int port, UDPFrame* frame);
    ~UDPResendObj();

    void update();

    std::string& ip() { return _ip; }
    int port() { return _port; }
    UDPFrame* frame() { return _frame; }
    long sendTsUs() const { return _sendTsUs; }
    int triedCnt() const { return _triedCnt; }
};

class UDPResendObjComp4MinHeap {
public:
    bool operator() (UDPResendObj* v1, UDPResendObj* v2) {
        return (v1->sendTsUs()) > (v2->sendTsUs());
    }
};

class UDPResendQueue {
private:
    SafePriorityQueue<UDPResendObj*, std::vector<UDPResendObj*>, UDPResendObjComp4MinHeap> _queue;

    static pthread_mutex_t _mutex;
    static UDPResendQueue* _instance;

    UDPResendQueue();
public:
    static UDPResendQueue* instance();

    UDPResendObj* top() { return _queue.top(); }

    void push(UDPResendObj* obj) { _queue.push(obj); }

    void pop() { _queue.pop(); }

};

class UDPSendQueue {
private:
    SafeQueue<UDPResendObj*>* _queue;

    static pthread_mutex_t _mutex;
    static UDPSendQueue* _instance;

    UDPSendQueue();
public:
    static UDPSendQueue* instance();

    void push(UDPResendObj* obj) { _queue->push(obj); }

    void pop() { _queue->pop(); }

};

class UDPAckMap {
private:
    // packetId + frameIndex, acked
    SafeMap<std::string, bool> _map;

    static pthread_mutex_t _mutex;
    static UDPAckMap* _instance;

    UDPAckMap();

public:
    static UDPAckMap* instance();

    bool isAcked(int packetId, byte frameIndex);
    void erase(int packetId, byte frameIndex);
    void setAcked(int packetId, byte frameIndex, bool acked);
    size_t size();
};

#endif /* UDPRESENDQUEUE_H_ */
