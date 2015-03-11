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

    byte* _frameSerializedBin;
    int _frameSerializedLen;

public:
    UDPResendObj(std::string ip, int port, UDPFrame* frame);
    ~UDPResendObj();

    void update();

    std::string& ip() { return _ip; }
    int port() { return _port; }
    UDPFrame* frame() { return _frame; }
    long sendTsUs() const { return _sendTsUs; }
    int triedCnt() const { return _triedCnt; }
    byte* frameSerializedBin() { return _frameSerializedBin; }
    int frameSerializedLen() { return _frameSerializedLen; }
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

//    static pthread_mutex_t _mutex;
//    static UDPResendQueue* _instance;


public:
    UDPResendQueue();
//    static UDPResendQueue* instance();

    UDPResendObj* pop() {
        if (_queue.size() == 0) {
            return NULL;
        }

        UDPResendObj* obj = _queue.top();
        _queue.pop();

        return obj;
    }


    void push(UDPResendObj* obj) { _queue.push(obj); }
};

class UDPSendQueue {
private:
    SafeQueue<UDPResendObj*>* _queue;

//    static pthread_mutex_t _mutex;
//    static UDPSendQueue* _instance;

public:
    UDPSendQueue();
//    static UDPSendQueue* instance();

    void push(UDPResendObj* obj) { _queue->push(obj); }

    UDPResendObj* pop() { return _queue->pop(); }

};

class UDPAckMap {
private:
    // packetId + frameIndex, acked
    SafeMap<std::string, bool> _map;

//    static pthread_mutex_t _mutex;
//    static UDPAckMap* _instance;



public:
    UDPAckMap();
//    static UDPAckMap* instance();

    bool needResend(int packetId, byte frameIndex);
    void erase(int packetId, byte frameIndex);

    void setNotAcked(int packetId, byte frameIndex);
    void setAckedIfExist(int packetId, byte frameIndex);
    size_t size();
};

#endif /* UDPRESENDQUEUE_H_ */
