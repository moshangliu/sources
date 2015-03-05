#include<cstring>

#include "UDPFrameHelper.h"
#include "UDPPacketIdCreator.h"

using namespace std;

tuple<byte*, int32> UDPFrameHelper::serialize(const UDPFrame* frame) {
    if (frame == NULL) {
        return make_tuple((byte*)NULL, 0);
    }

    if (frame->version() != VERSION_0) {
        return make_tuple((byte*)NULL, 0);
    }

    return serialize_0(frame);
}

tuple<byte*, int32> UDPFrameHelper::serialize_0(const UDPFrame* frame) {
    int32 BUF_LEN = UDP_FRAME_HEADER_LEN_FOR_VERSION_0 + frame->contentLength();
    byte* data = new byte[BUF_LEN];
    memset(data, 0, BUF_LEN);

    data[0] = frame->version();
    data[1] = frame->packetType();

    tuple<byte, byte, byte, byte> t = int32To4Bytes(frame->packetId());
    data[2] = get<0>(t);
    data[3] = get<1>(t);
    data[4] = get<2>(t);
    data[5] = get<3>(t);

    data[6] = frame->frameCount();
    data[7] = frame->frameIndex();

    tuple<byte, byte> ts = int16To2Bytes(frame->contentLength());
    data[8] = get<0>(ts);
    data[9] = get<1>(ts);

    if (frame->contentLength() > 0) {
        memcpy(data + UDP_FRAME_HEADER_LEN_FOR_VERSION_0, frame->content(), frame->contentLength());
    }
    return make_tuple(data, BUF_LEN);
}

UDPFrame* UDPFrameHelper::unserialize(const byte* data, int len) {
    if (data == NULL || len < UDP_FRAME_HEADER_LEN_FOR_VERSION_0) {
        return NULL;
    }

    byte version = data[0];
    if (version == VERSION_0) {
        return unserialize_0(data, len);
    }

    return NULL;
}

UDPFrame* UDPFrameHelper::unserialize_0(const byte* data, int len) {
    byte version = data[0];
    if (version != VERSION_0) {
        return NULL;
    }

    byte packetType = data[1];
    if (packetType != UDPPacketType::Packet && packetType != UDPPacketType::Ack) {
        return NULL;
    }

    int32 packetId = toInt32(data[2], data[3], data[4], data[5]);
    byte frameCount = data[6];
    byte frameIndex = data[7];
    int16 contentLen = toInt16(data[8], data[9]);
    if (len != contentLen + UDP_FRAME_HEADER_LEN_FOR_VERSION_0) {
        return NULL;
    }

    if (packetType == UDPPacketType::Ack) {
        return UDPFrame::buildAck(version, packetId, frameCount, frameIndex);
    }

    byte* content = NULL;
    if (contentLen > 0) {
        content = new byte[contentLen];
        memcpy(content, data + UDP_FRAME_HEADER_LEN_FOR_VERSION_0, contentLen);
    }

    return UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, contentLen, content);
}

vector<UDPFrame*>* UDPFrameHelper::segment(const byte* data, int len) {

    if (data == NULL || len <= 0 || len > UDP_PACKET_MAX_SIZE) {
        return NULL;
    }

    int frameCount = len / UDP_FRAME_MAX_SIZE;
    if (len % UDP_FRAME_MAX_SIZE > 0) {
        frameCount++;
    }

    const int packetId = UDPPacketIdCreator::instance()->next();
    vector<UDPFrame*>* frames = new vector<UDPFrame*>();
    int idx = 0;
    for (int i = 0; i < frameCount; i++) {
        int frameContentLen = i == frameCount - 1 ? len - idx : UDP_FRAME_MAX_SIZE;
        byte* frameContent = new byte[frameContentLen];
        memcpy(frameContent, data + idx, frameContentLen);
        idx += frameContentLen;

        int version = VERSION_0;
        int frameIndex = i + 1;

        UDPFrame* frame = UDPFrame::buildPacket(version, packetId, frameCount, frameIndex, frameContentLen, frameContent);
        frames->push_back(frame);
    }

    return frames;
}
