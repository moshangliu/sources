#include <cstdio>
#include <cstring>
#include <tuple>

#include "MutexLock.h"
#include "UDPRecvDataStructs.h"

using namespace std;

UDPRecvObj::UDPRecvObj(std::string ip, int port, int packetId, byte frameCount)
    : _ip(ip), _port(port), _packetId(packetId), _frameCount(frameCount){
}

UDPRecvObj::~UDPRecvObj() {
    for (map<byte, UDPFrame*>::iterator it = _frames.begin(); it != _frames.end(); it++) {
        UDPFrame* frame = it->second;
        delete frame;
    }
}

void UDPRecvObj::put(UDPFrame* frame) {
    if (frame == NULL) {
        return;
    }

    byte frameIndex = frame->frameIndex();

    map<byte, UDPFrame*>::iterator it = _frames.find(frameIndex);
    if (_frames.find(frameIndex) != _frames.end()) {
        UDPFrame* oldFrame = it->second;
        delete oldFrame;
    }

    _frames[frameIndex] = frame;
}

bool UDPRecvObj::completed() {
    return _frameCount == _frames.size();
}

std::tuple<byte*, int> UDPRecvObj::assemble() {
    if (_frameCount != _frames.size()) {
        return make_tuple((byte*)NULL, 0);
    }

    int contentLen = 0;
    for (int i = 1; i <= _frameCount; i++) {
        map<byte, UDPFrame*>::iterator it = _frames.find(i);
        if (it == _frames.end()) {
            return make_tuple((byte*)NULL, 0);
        }
        contentLen += it->second->contentLength();
    }

    byte* data = new byte[contentLen];
    memset(data, 0, contentLen);

    int idx = 0;
    for (int i = 1; i <= _frameCount; i++) {
        map<byte, UDPFrame*>::iterator it = _frames.find(i);
        UDPFrame* frame = it->second;
        memcpy(data + idx, frame->content(), frame->contentLength());
        idx += frame->contentLength();
    }

    return make_tuple(data, contentLen);
}

pthread_mutex_t UDPRecvContainer::_mutex4instance = PTHREAD_MUTEX_INITIALIZER;
UDPRecvContainer* UDPRecvContainer::_instance = NULL;

UDPRecvContainer::UDPRecvContainer() {
    _mutex4container = PTHREAD_MUTEX_INITIALIZER;
}

UDPRecvContainer* UDPRecvContainer::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex4instance);
        if (_instance == NULL) {
            _instance = new UDPRecvContainer();
        }
    }

    return _instance;
}

tuple<bool, byte*, int> UDPRecvContainer::putOrAssemble(string ip, int port, UDPFrame* frame) {
    if (frame == NULL) {
        return make_tuple(false, (byte*)NULL, 0);
    }

    int packetId = frame->packetId();
    string packetKey = makeRecvPacketKey(ip, port, packetId);

    MutexLock lock(&_mutex4container);

    if (!_udpRecvObjs.has(packetKey)) {
        UDPRecvObj* obj = new UDPRecvObj(ip, port, packetId, frame->frameCount());
        _udpRecvObjs.put(packetKey, obj);
        _udpRecvMetas.push(new UDPRecvObjMeta(ip, port, packetId));
    }

    UDPRecvObj* obj = _udpRecvObjs.get(packetKey);
    obj->put(frame);

    if (!obj->completed()) {
        return make_tuple(false, (byte*)NULL, 0);
    }

    tuple<byte*, int> content = obj->assemble();
    delete obj;
    _udpRecvObjs.erase(packetKey);

    return make_tuple(true, get<0>(content), get<1>(content));
}

int UDPRecvContainer::expireOrReturnSleepUs() {
    const int DEFAULT_SLEEP_US = 10;

    MutexLock lock(&_mutex4container);
    if (_udpRecvMetas.empty()) {
        return DEFAULT_SLEEP_US;
    }

    UDPRecvObjMeta* meta = _udpRecvMetas.top();
    long current = current_us();
    if (meta->expireTsUs() > current) {
        return meta->expireTsUs() - current;
    }

    _udpRecvMetas.pop();
    string packetKey = meta->packetKey();
    if (!_udpRecvObjs.has(packetKey)) {
        delete meta;
        return 0;
    }

    UDPRecvObj* obj = _udpRecvObjs.get(packetKey);
    delete obj;

    delete meta;
    return 0;
}
