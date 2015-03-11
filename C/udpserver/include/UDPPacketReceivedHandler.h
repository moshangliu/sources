/*
 * UDPPacketReceivedHandler.h
 *
 *  Created on: 2015Äê3ÔÂ10ÈÕ
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
