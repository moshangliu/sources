#ifndef UDP_FRAME_H
#define UDP_FRAME_H

#include "Common.h"

/*
 * version == 0
 * | byte    | byte | 4bytes   | byte       | byte         | 2bytes        | ...
 * | version | type | packetId | frame_cnt  | frame_index  | contentLength | ...
 */

enum UDPPacketType {
    Ack = 0,
    Packet = 1,
    UNKNOWN = 127
};

class UDPFrame {
private:
    byte _version;
    byte _packetType;
    int32 _packetId;
    byte _frameCount;
    byte _frameIndex;
    int16 _contentLength;

    byte* _content;

    UDPFrame(byte version, byte packetType, int32 packetId, byte frameCount, byte frameIndex, int16 contentLength, byte* content)
        : _version(version), _packetType(packetType), _packetId(packetId), _frameCount(frameCount),
          _frameIndex(frameIndex), _contentLength(contentLength), _content(content) { }

public:
    ~UDPFrame() {
        delete[] _content;
    }

    byte version() const { return _version; }

    UDPPacketType packetType() const {
        if (_packetType == UDPPacketType::Ack) {
            return UDPPacketType::Ack;
        }
        if (_packetType == UDPPacketType::Packet) {
            return UDPPacketType::Packet;
        }
        return UDPPacketType::UNKNOWN;
    }

    int32 packetId() const { return _packetId; }

    byte frameCount() const { return _frameCount; }

    byte frameIndex() const { return _frameIndex; }

    int16 contentLength() const { return _contentLength; }

    const byte* const content() const { return _content; }

    static UDPFrame* buildPacket(byte version, int32 packetId, byte frameCount, byte frameIndex, int16 contentLength, byte* content) {
        return new UDPFrame(version, UDPPacketType::Packet, packetId, frameCount, frameIndex, contentLength, content);
    }

    static UDPFrame* buildAck(byte version, int32 packetId, byte frameCount, byte frameIndex) {
        return new UDPFrame(version, UDPPacketType::Ack, packetId, frameCount, frameIndex, 0, NULL);
    }

    UDPFrame* buildAck() {
        return buildAck(_version, _packetId, _frameCount, _frameIndex);
    }
};

#endif
