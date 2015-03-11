#ifndef UDPRESENDQUEUE_H_
#define UDPRESENDQUEUE_H_

#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>

#include "UDPFrame.h"
#include "SafeQueue.h"
#include "SafeMap.h"
#include "MutexLock.h"

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
    std::priority_queue<UDPResendObj*, std::vector<UDPResendObj*>, UDPResendObjComp4MinHeap> _queue;
    pthread_mutex_t _mutex;
public:
    UDPResendQueue() {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    UDPResendObj* pop() {
        MutexLock lock(&_mutex);

        if (_queue.size() == 0) {
            return NULL;
        }

        UDPResendObj* obj = _queue.top();
        _queue.pop();

        return obj;
    }


    void push(UDPResendObj* obj) {
        MutexLock lock(&_mutex);
        _queue.push(obj);
    }
};

class UDPSendQueue {
private:
    SafeQueue<UDPResendObj*>* _queue;

public:
    UDPSendQueue();

    void push(UDPResendObj* obj) { _queue->push(obj); }

    UDPResendObj* pop() { return _queue->pop(); }

};

class UDPAckMap {
private:
    // packetId + frameIndex, acked
    std::map<std::string, bool> _map;
    pthread_mutex_t _mutex;

public:
    UDPAckMap() {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    bool needResend(int packetId, byte frameIndex);
    void erase(int packetId, byte frameIndex);

    void setNotAcked(int packetId, byte frameIndex);
    void setAckedIfExist(int packetId, byte frameIndex);
    size_t size();
};

class UDPPacketObj {
private:
    std::string _ip;
    int16 _port;

    byte _type;

    std::set<byte> _frameIndexesNotActed;
    byte* _rawData;
    int _rawDataLen;

public:
    UDPPacketObj(std::string ip, int16 port, byte type,
        std::set<byte> frameIndexesNotActed, byte* raw, int rawDataLen)
    : _ip(ip), _port(port), _type(type),
      _frameIndexesNotActed(frameIndexesNotActed),
      _rawData(raw), _rawDataLen(rawDataLen) {}

    ~UDPPacketObj() {
        delete [] _rawData;
    }

    bool sendSuccess() {
        return _frameIndexesNotActed.size() == 0;
    }

    void ack(byte frameIndex) {
        _frameIndexesNotActed.erase(frameIndex);
    }

    std::string ip() {
        return _ip;
    }

    int16 port() {
        return _port;
    }

    byte type() {
        return _type;
    }

    byte* rawData() {
        return _rawData;
    }

    int rawDataLen() {
        return _rawDataLen;
    }
};

class UDPPacketMap {
private:
    // packetId, UDPPacketObj
    std::map<int, UDPPacketObj*> _map;
    pthread_mutex_t _mutex;

public:
    UDPPacketMap() {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    void put(int packetId, UDPPacketObj* obj) {
        MutexLock lock(&_mutex);

        _map[packetId] = obj;
    }

    UDPPacketObj* ackAndIfSuccessThenPop(int packetId, byte frameIndex) {
        MutexLock lock(&_mutex);

        std::map<int, UDPPacketObj*>::iterator it = _map.find(packetId);
        if (it == _map.end()) {
            return NULL;
        }

        UDPPacketObj* result = it->second;
        result->ack(frameIndex);
        if (result->sendSuccess()) {
            _map.erase(it);
            return result;
        }

        return NULL;
    }

    UDPPacketObj* ifExistThenPop(int packetId) {
        MutexLock lock(&_mutex);

        std::map<int, UDPPacketObj*>::iterator it = _map.find(packetId);
        if (it == _map.end()) {
            return NULL;
        }

        UDPPacketObj* result = it->second;
        _map.erase(it);

        return result;
    }
};

#endif /* UDPRESENDQUEUE_H_ */
