#include "Common.h"
#include "UDPSendDataStructs.h"
#include "UDPRetryTimeSpan.h"
#include "MutexLock.h"

using namespace std;

UDPResendObj::UDPResendObj(string ip, int port, UDPFrame* frame)
    : _ip(ip), _port(port), _frame(frame), _triedCnt(0), _sendTsUs(current_us()) {}

UDPResendObj::~UDPResendObj() {
    delete _frame;
}

void UDPResendObj::update() {
    _triedCnt++;
    _sendTsUs += UDPRetryTimeSpan::instance()->waitTimeUs(_triedCnt);
}

UDPSendQueue* UDPSendQueue::_instance = NULL;
pthread_mutex_t UDPSendQueue::_mutex = PTHREAD_MUTEX_INITIALIZER;

UDPSendQueue::UDPSendQueue() {
    _queue = new SafeQueue<UDPResendObj*>(1024*1024*1024);
}

UDPSendQueue* UDPSendQueue::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex);
        if (_instance == NULL) {
            _instance = new UDPSendQueue();
        }
    }

    return _instance;
}


UDPResendQueue* UDPResendQueue::_instance = NULL;
pthread_mutex_t UDPResendQueue::_mutex = PTHREAD_MUTEX_INITIALIZER;

UDPResendQueue::UDPResendQueue() {}
UDPResendQueue* UDPResendQueue::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex);
        if (_instance == NULL) {
            _instance = new UDPResendQueue();
        }
    }

    return _instance;
}

UDPAckMap* UDPAckMap::_instance = NULL;
pthread_mutex_t UDPAckMap::_mutex = PTHREAD_MUTEX_INITIALIZER;

UDPAckMap::UDPAckMap() {}
UDPAckMap* UDPAckMap::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex);
        if (_instance == NULL) {
            _instance = new UDPAckMap();
        }
    }

    return _instance;
}

void UDPAckMap::setNotAcked(int packetId, byte frameIndex) {
    string key = makeKey(packetId, frameIndex);
    _map.put(key, false);
}

void UDPAckMap::setAckedIfExist(int packetId, byte frameIndex) {
    string key = makeKey(packetId, frameIndex);
    if (_map.has(key)) {
        _map.put(key, true);
    }
}

bool UDPAckMap::isAcked(int packetId, byte frameIndex) {
    string key = makeKey(packetId, frameIndex);

    // 如果key不存在，说明已经Ack，并将其删除，所以返回true
    if (!_map.has(key)) {
        return true;
    }

    return _map.get(key);
}

void UDPAckMap::erase(int packetId, byte frameIndex) {
    string key = makeKey(packetId, frameIndex);
    _map.erase(key);

}

size_t UDPAckMap::size() {
    return _map.size();
}
