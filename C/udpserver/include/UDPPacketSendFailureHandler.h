#ifndef UDP_PACKET_SEND_SUCCESS_HANDLER
#define UDP_PACKET_SEND_SUCCESS_HANDLER

#include <string>
#include "Common.h"

class UDPPacketSendFailureHandler {
public:
    /**
     * @param, ip: remoteIp
     * @param, port: remotePort
     * @param, type: eventType
     * @param, content: content sent to ip:port, will be deleted after calling handle
     * @param, contentLen: length of content
     **/
    virtual void handle(std::string ip, int port, byte type,
        byte* content, int contentLen) {

    }

    virtual ~UDPPacketSendFailureHandler() {

    }
};

#endif
