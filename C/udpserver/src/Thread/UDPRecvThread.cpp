#include <iostream>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "boost/smart_ptr.hpp"
#include "LoggerWrapper.h"
#include "UDPRecvDataStructs.h"
#include "UDPSendDataStructs.h"
#include "UDPFrameHelper.h"
#include "UDPRecvThread.h"

using namespace std;
using namespace boost;

typedef struct sockaddr sockaddr_t;

UDPRecvThread::UDPRecvThread(uint32 port, UDPPacketDispatcher* dispatcher,
    UDPAckMap* ackMap, UDPRecvContainer* recvContainer, UDPPacketMap* packetMap,
    UDPPacketSendSuccessHandler* successHandler)
    : _port(port), _dispatcher(dispatcher), _ackMap(ackMap),
      _recvContainer(recvContainer),_packetMap(packetMap),
      _successHandler(successHandler),
      _stopFlag(false) {
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
    while (!_stopFlag) {
        const int BUF_LEN = 2048; // UDP_FRAME_MAX_SIZE;

        shared_array<byte> buf(new byte[BUF_LEN]);
        memset(buf.get(), 0, BUF_LEN);

        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int n = recvfrom(listenfd, buf.get(), BUF_LEN, 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (n <= 0) {
            LoggerWrapper::instance()->warn("UDPTrace, RECV, Failed");
            continue;
        }

        UDPFrame* frame = UDPFrameHelper::unserialize(buf.get(), n);
        if (frame == NULL) {
            LoggerWrapper::instance()->warn("UDPTrace, RECV, INVALID_FRAME, contentLen:%d", n);
            continue;
        }

        const int packetId = frame->packetId();
        const byte frameCount = frame->frameCount();
        const byte frameIndex = frame->frameIndex();

        string ip = string(inet_ntoa(clientAddr.sin_addr));
        short port = ntohs(clientAddr.sin_port);
        if (frame->packetType() == UDPPacketType::Ack) {
            _ackMap->setAckedIfExist(frame->packetId(), frame->frameIndex());
            UDPPacketObj* packetObj = _packetMap->ackAndIfSuccessThenEraseAndReturn(packetId, frameIndex);
            if (packetObj != NULL) {
                if (_successHandler != NULL) {
                    _successHandler->handle(packetObj->ip(), packetObj->port(), packetObj->type(),
                        packetObj->rawData(), packetObj->rawDataLen());
                }
                LoggerWrapper::instance()->debug("UDPTrace, SEND_PACKET_SUCCESS, PacketId:%d, TO:%s-%d",
                    packetId, ip.c_str(), port);
                delete packetObj;
            }

            LoggerWrapper::instance()->debug("UDPTrace, RECV_ACK, PacketId:%d, %d/%d, From:%s-%d",
                packetId, frameIndex, frameCount, ip.c_str(), port);

            delete frame;
            continue;
        }
        if (frame->packetType() == UDPPacketType::Packet) {
            LoggerWrapper::instance()->debug("UDPTrace, RECV_FRAME, PacketId:%d, %d/%d, ContentLen:%d, From:%s-%d",
                packetId, frameIndex, frameCount, frame->contentLength(), ip.c_str(), port);

            sendAck(frame, &clientAddr);
            LoggerWrapper::instance()->debug("UDPTrace, SEND_ACK, PacketId:%d, %d/%d, To:%s-%d",
                packetId, frameIndex, frameCount, ip.c_str(), port);

            tuple<bool, byte*, int> ret = _recvContainer->putOrAssemble(ip, port, frame);
            if (!get<0>(ret)) {
                continue;
            }

            shared_array<byte> content(get<1>(ret));
            int contentLen = get<2>(ret);

            _dispatcher->dispatch(ip, port, content.get(), contentLen);

            LoggerWrapper::instance()->debug("UDPTrace, RECV_PACKET_COMPLETE, PacketId:%d, ContentLen:%d", packetId, contentLen);
        }
    }
}

void UDPRecvThread::sendAck(UDPFrame* frame, struct sockaddr_in* clientAddr) {
    if (frame == NULL || clientAddr == NULL) {
        return;
    }

    boost::shared_ptr<UDPFrame> ack(frame->buildAck());
    tuple<byte*, int> ackTuple = UDPFrameHelper::serialize(ack.get());
    shared_array<byte> ackBin(get<0>(ackTuple));
    int ackBinLen = get<1>(ackTuple);

    sendto(_listenfd, ackBin.get(), ackBinLen,
        0,
        (struct sockaddr*)clientAddr, sizeof(sockaddr_in));
}

void* UDPRecvThread::process() {
    if (_port < 0) {
        cerr << "You should bind port > 0" << endl;
        exit(1);
    }

    LoggerWrapper::instance()->info("UDPRecvThread started");
    Bind(_listenfd);
    accept(_listenfd);

    LoggerWrapper::instance()->info("UDPRecvThread stopped");
    return NULL;
}
