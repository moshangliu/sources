/*
 * UDPPacketSendSuccessHandler.h
 *
 *  Created on: 2015��3��11��
 *      Author: lrt
 */

#ifndef UDPPACKETSENDSUCCESSHANDLER_H_
#define UDPPACKETSENDSUCCESSHANDLER_H_

#include <string>
#include "Common.h"

class UDPPacketSendSuccessHandler {
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

    virtual ~UDPPacketSendSuccessHandler() {

    }
};


#endif /* UDPPACKETSENDSUCCESSHANDLER_H_ */
