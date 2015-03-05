#ifndef UDP_FRAME_HELPER_H
#define UDP_FRAME_HELPER_H

#include <vector>
#include <tuple>
#include "UDPFrame.h"

class UDPFrameHelper {
private:
    static std::tuple<byte*, int32> serialize_0(const UDPFrame* frame);
    static UDPFrame* unserialize_0(const byte* data, int len);

public:
    static std::tuple<byte*, int32> serialize(const UDPFrame* frame);
    static UDPFrame* unserialize(const byte* data, int len);
    static std::vector<UDPFrame*>* segment(const byte* data, int len);
};

#endif
