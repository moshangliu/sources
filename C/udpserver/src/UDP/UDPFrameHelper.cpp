#include<cstring>

#include "UDPFrameHelper.h"

using namespace std;

tuple<byte*, int32> UDPFrameHelper::serialize(UDPFrame* frame) {
    if (frame == NULL) {
        return make_tuple((byte*)NULL, 0);
    }

    if (frame->version() != VERSION_0) {
        return make_tuple((byte*)NULL, 0);
    }

    return serialize_0(frame);
}

tuple<byte*, int32> UDPFrameHelper::serialize_0(UDPFrame* frame) {
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

    memcpy(data + UDP_FRAME_HEADER_LEN_FOR_VERSION_0, frame->content(), frame->contentLength());
    return make_tuple(data, BUF_LEN);
}

UDPFrame* UDPFrameHelper::unserialize(byte* data, int len) {
    if (data == NULL || len < UDP_FRAME_HEADER_LEN_FOR_VERSION_0) {
        return NULL;
    }

    return NULL;
}

UDPFrame* UDPFrameHelper::unserialize_0(byte* data, int len) {
    return unserialize(data, len);
}
