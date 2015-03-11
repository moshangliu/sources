#ifndef UDPPACKETDISPATCHER_H_
#define UDPPACKETDISPATCHER_H_

#include <map>
#include <string>

#include "Common.h"
#include "UDPPacketReceivedHandler.h"

class UDPPacketDispatcher {
private:
    std::map<byte, UDPPacketReceivedHandler*> _handlers;
    UDPPacketReceivedHandler* _defaultHandler;

public:
    UDPPacketDispatcher();
    void registerHandler(byte type, UDPPacketReceivedHandler* handler);
    void unregisterHandler(byte type);
    void setDefaultHandler(UDPPacketReceivedHandler* handler);
    UDPPacketReceivedHandler* getHandler(byte type);

    void dispatch(std::string ip, int port, byte* content, int contentLen);
};


#endif /* UDPPACKETDISPATCHER_H_ */
