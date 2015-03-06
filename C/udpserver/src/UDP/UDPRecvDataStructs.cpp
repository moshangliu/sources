#include <cstdio>
#include <cstring>

#include "MutexLock.h"
#include "UDPRecvDataStructs.h"

using namespace std;

UDPRecvObj::UDPRecvObj(std::string ip, int port, int packetId, byte frameCount)
    : _ip(ip), _port(port), _packetId(packetId), _frameCount(frameCount){
    _mutex = PTHREAD_MUTEX_INITIALIZER;
}

UDPRecvObj::~UDPRecvObj() {
    for (map<byte, UDPFrame*>::iterator it = _frames.begin(); it != _frames.end(); it++) {
        UDPFrame* frame = it->second;
        delete frame;
    }
}

void UDPRecvObj::put(UDPFrame* frame) {
    MutexLock lock(&_mutex);
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
    MutexLock lock(&_mutex);
    return _frameCount == _frames.size();
}

std::tuple<byte*, int> UDPRecvObj::assemble() {
    MutexLock lock(&_mutex);
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
