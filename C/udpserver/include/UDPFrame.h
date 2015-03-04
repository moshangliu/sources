#ifndef UDP_FRAME_H
#define UDP_FRAME_H


enum UDPPacketType {Ack(UDP_ACK_TYPE), Packet(UDP_PACKET_TYPE), UNKNOWN(UDP_UNKNOWN_TYPE)};

class UDPFrame {
private:
    byte _version;
    byte _packetType;
    int32 _packetId;
    byte _frameCount;
    byte _frameIndex;
    int16 _contentLength;

    byte* content;

public:
    UDPFrame(byte version, byte packetType, int32 packetId, byte frameCount, byte frameIndex, int16 contentLength, byte* content)
        : _version(version), _packetType(packetType), _packetId(packetId), _frameCount(frameCount),
          _frameIndex(frameIndex), _contentLength(contentLength), _content(content) { }

    ~UDPFrame() {
        delete[] content;
    }

    byte version() { return _version; }

    UDPPacketType packetType() {
        if (_packetType == UDP_ACK_TYPE) {
            return UDPPacketType::Ack;
        }
        if (_packetType == UDP_PACKET_TYPE) {
            return UDPPacketType::Packet;
        }
        return UDPPacketType::UNKNOWN;
    }

    int32 packetId() { return _packetId; }

    byte frameCount() { return _frameCount; }

    byte frameIndex() { return _frameIndex; }

    int16 contentLength() { return _contentLength; }

    const byte* const content() { return _content; }
};

#endif