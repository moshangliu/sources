/*
 * UDPPacketReceivedHandler.h
 *
 *  Created on: 2015��3��10��
 *      Author: lrt
 */

#ifndef UDPPACKETRECEIVEDHANDLER_H_
#define UDPPACKETRECEIVEDHANDLER_H_

#include <string>

class UDPPacketReceivedHandler {
public:
    /**
     * @param: content, will be delete outside
     **/
    virtual void handle(std::string ip, int port, byte* content, int contentLen) {

    }

    virtual ~UDPPacketReceivedHandler() {

    }
};


#endif /* UDPPACKETRECEIVEDHANDLER_H_ */
