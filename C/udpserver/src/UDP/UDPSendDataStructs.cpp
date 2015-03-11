#include "Common.h"
#include "UDPFrameHelper.h"
#include "UDPSendDataStructs.h"
#include "UDPRetryTimeSpan.h"
#include "MutexLock.h"

using namespace std;

UDPResendObj::UDPResendObj(string ip, int port, UDPFrame* frame)
    : _ip(ip), _port(port), _frame(frame), _triedCnt(0), _sendTsUs(current_us()) {

    tuple<byte*, int> t = UDPFrameHelper::serialize(frame);
    _frameSerializedBin = get<0>(t);
    _frameSerializedLen = get<1>(t);
}

UDPResendObj::~UDPResendObj() {
    delete _frame;
    delete [] _frameSerializedBin;
}

void UDPResendObj::update() {
    _triedCnt++;
    _sendTsUs += UDPRetryTimeSpan::instance()->waitTimeUs(_triedCnt);
}

UDPSendQueue::UDPSendQueue() {
    _queue = new SafeQueue<UDPResendObj*>(1024*1024*1024);
}

void UDPAckMap::setNotAcked(int packetId, byte frameIndex) {
    MutexLock lock(&_mutex);

    string key = makeKey(packetId, frameIndex);
    _map[key] = false;
}

void UDPAckMap::setAckedIfExist(int packetId, byte frameIndex) {
    MutexLock lock(&_mutex);

    string key = makeKey(packetId, frameIndex);
    if (_map.find(key) != _map.end()) {
        _map[key] = true;
    }
}

bool UDPAckMap::needResend(int packetId, byte frameIndex) {
    MutexLock lock(&_mutex);

    string key = makeKey(packetId, frameIndex);
    return _map.find(key) != _map.end() && !_map[key]; // Not acked
}

void UDPAckMap::erase(int packetId, byte frameIndex) {
    MutexLock lock(&_mutex);

    string key = makeKey(packetId, frameIndex);
    _map.erase(key);
}

size_t UDPAckMap::size() {
    return _map.size();
}
