#ifndef UDP_FRAME_HELPER_H
#define UDP_FRAME_HELPER_H

class UDPFrameHelper {
private:
    static byte* serialize_0(UDPFrame* frame) {

    }

public:
    static byte* serialize(UDPFrame* frame) {
        if (frame == NULL) {
            return NULL;
        }
        if (frame->version() == 0) {
            return serialize_0(frame);
        }
        return frame;
    }

    static UDPFrame* unserialize(byte[] data) {
        if (data == NULL) {
            return NULL;
        }
    }
};

#endif