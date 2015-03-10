#include <iostream>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "LoggerWrapper.h"
#include "UDPRecvDataStructs.h"
#include "UDPSendDataStructs.h"
#include "UDPFrameHelper.h"
#include "UDPRecvThread.h"

using namespace std;

typedef struct sockaddr sockaddr_t;

UDPRecvThread::UDPRecvThread(uint32 port) : _port(port) {
    _listenfd = 0;
    if ((_listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "Failed to create listen socket." << endl;
        exit(2);
    }
}

struct sockaddr_in* UDPRecvThread::getSvrAddr() {
    struct sockaddr_in* svrAddr = new sockaddr_in();
    memset(svrAddr, 0, sizeof(struct sockaddr_in));
    svrAddr->sin_family = AF_INET;
    svrAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    svrAddr->sin_port = htons(_port);

    return svrAddr;
}

void UDPRecvThread::Bind(int listenfd) {
    struct sockaddr_in* svrAddr = getSvrAddr();
    if (bind(listenfd, (struct sockaddr*)svrAddr, sizeof(struct sockaddr_in)) < 0){
        cerr << "Failed to bind port [" << _port << "]" << endl;
        exit(3);
    }
}

void UDPRecvThread::accept(int listenfd) {
    while (true) {
        const int BUF_LEN = 2048; // UDP_FRAME_MAX_SIZE;
        byte buf[2048]{0};

        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int n = recvfrom(listenfd, buf, BUF_LEN, 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (n <= 0) {
            LoggerWrapper::instance()->warn("UDPTrace, RECV, Failed");
            continue;
        }

        UDPFrame* frame = UDPFrameHelper::unserialize(buf, n);
        if (frame == NULL) {
            LoggerWrapper::instance()->warn("UDPTrace, RECV, INVALID_FRAME");
           continue;
        }

        const int packetId = frame->packetId();
        const byte frameCount = frame->frameCount();
        const byte frameIndex = frame->frameIndex();
        if (frame->packetType() == UDPPacketType::Ack) {
            LoggerWrapper::instance()->debug("UDPTrace, RECV_ACK, PacketId:%d, %d/%d",
                packetId, frameIndex, frameCount);
            UDPAckMap::instance()->setAckedIfExist(frame->packetId(), frame->frameIndex());
            delete frame;

            continue;
        }
        if (frame->packetType() == UDPPacketType::Packet) {
            LoggerWrapper::instance()->debug("UDPTrace, RECV_FRAME, PacketId:%d, %d/%d, ContentLen:%d",
                packetId, frameIndex, frameCount, frame->contentLength());

            sendAck(frame, &clientAddr);
            LoggerWrapper::instance()->debug("UDPTrace, SEND_ACK, PacketId:%d, %d/%d",
                packetId, frameIndex, frameCount);

            string ip = string(inet_ntoa(clientAddr.sin_addr));
            short port = ntohs(clientAddr.sin_port);
            tuple<bool, byte*, int> ret = UDPRecvContainer::instance()->putOrAssemble(ip, port, frame);
            if (!get<0>(ret)) {
                continue;
            }

            byte* content = get<1>(ret);
            int contentLen = get<2>(ret);

            LoggerWrapper::instance()->debug("UDPTrace, RECV_PACKET_COMPLETE, PacketId:%d, ContentLen:%d", packetId, contentLen);
            delete [] content;
        }
    }
}

void UDPRecvThread::sendAck(UDPFrame* frame, struct sockaddr_in* clientAddr) {
    if (frame == NULL || clientAddr == NULL) {
        return;
    }

    UDPFrame* ack = frame->buildAck();
    tuple<byte*, int> ackTuple = UDPFrameHelper::serialize(ack);
    byte* ackBin = get<0>(ackTuple);
    int ackBinLen = get<1>(ackTuple);

    sendto(_listenfd, ackBin, ackBinLen,
        0,
        (struct sockaddr*)clientAddr, sizeof(sockaddr_in));
    delete ack;
    delete [] ackBin;
}

void* UDPRecvThread::process() {
    if (_port < 0) {
        cerr << "You should bind port > 0" << endl;
        exit(1);
    }

    LoggerWrapper::instance()->info("UDPRecvThread started");
    Bind(_listenfd);
    accept(_listenfd);

    return NULL;
}
