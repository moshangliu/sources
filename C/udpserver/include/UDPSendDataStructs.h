#ifndef UDPRESENDQUEUE_H_
#define UDPRESENDQUEUE_H_

#include <vector>
#include <string>
#include "UDPFrame.h"
#include "SafePriorityQueue.h"

class UDPResendObj {
private:
    UDPFrame* _frame;
    int _triedCnt;
    long _sendTsUs;
public:
    UDPResendObj(UDPFrame* frame);
    ~UDPResendObj();

    void update();

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

/*class UDPAckMap {
private:
    // packetId + frameIndex, acked
    SafeMap<string, bool> _map;

    static pthread_mutex_t _mutex;
    static UDPResendQueue* _instance;

    UDPAckMap();
public:

};*/

#endif /* UDPRESENDQUEUE_H_ */
