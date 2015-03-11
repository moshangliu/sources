#include "UDPPacketDispatcher.h"
#include "LoggerWrapper.h"
#include <cstring>

using namespace std;

UDPPacketDispatcher::UDPPacketDispatcher() {
    _defaultHandler = NULL;
}

void UDPPacketDispatcher::registerHandler(byte type, UDPPacketReceivedHandler* handler) {
    unregisterHandler(type);
    _handlers[type] = handler;
}

void UDPPacketDispatcher::unregisterHandler(byte type) {
    map<byte, UDPPacketReceivedHandler*>::iterator it = _handlers.find(type);
    if (it == _handlers.end()) {
        return;
    }

    UDPPacketReceivedHandler* handler = it->second;
    delete handler;

    _handlers.erase(it);
}

void UDPPacketDispatcher::setDefaultHandler(UDPPacketReceivedHandler* handler) {
    _defaultHandler = handler;
}

UDPPacketReceivedHandler* UDPPacketDispatcher::getHandler(byte type) {
    if (_handlers.find(type) == _handlers.end()) {
        return _defaultHandler;
    }

    return _handlers[type];
}

void UDPPacketDispatcher::dispatch(std::string ip, int port, byte* content, int contentLen) {
    if (content == NULL || contentLen <= 0) {
        return;
    }

    byte type = content[0];
    byte* realContent = new byte[contentLen - 1];
    memcpy(realContent, content + 1, contentLen - 1);

    UDPPacketReceivedHandler* handler = getHandler(type);
    if (handler == NULL) {
        LoggerWrapper::instance()->warn("UDPTrace, NO_DISPATCHER, TYPE:%d", type);
        delete [] realContent;

        return;
    }

    handler->handle(ip, port, realContent, contentLen - 1);
}
