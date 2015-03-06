#ifndef UDPRETRYTIMESPAN_H_
#define UDPRETRYTIMESPAN_H_

#include <tuple>
#include <vector>

#include "Common.h"

class UDPRetryTimeSpan {
private:
    std::tuple<int, int> ZERO;
    std::tuple<int, int> ONE;
    std::tuple<int, int> TWO;
    std::tuple<int, int> THREE;
    std::tuple<int, int> FOUR;
    std::tuple<int, int> FIVE;
    std::tuple<int, int> SIX;

    const int DEFAULT_TIME_SPAN_US = 10 * 1000; // 10ms
    std::vector<std::tuple<int, int>> vec;

    static pthread_mutex_t _mutex;
    static UDPRetryTimeSpan* _instance;
private:
    UDPRetryTimeSpan();

public:
    static UDPRetryTimeSpan* instance();
    int maxTryCnt();
    int allWaitTimeUs();
    int waitTimeUs(int triedCnt);
};



#endif /* UDPRETRYTIMESPAN_H_ */
