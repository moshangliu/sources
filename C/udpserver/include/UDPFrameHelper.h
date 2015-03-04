#ifndef UDP_FRAME_HELPER_H
#define UDP_FRAME_HELPER_H

#include <tuple>

class UDPFrameHelper {
private:
    static byte* serialize_0(UDPFrame* frame);

public:
    static byte* serialize(UDPFrame* frame);

    static UDPFrame* unserialize(byte* data);
};

#endif
