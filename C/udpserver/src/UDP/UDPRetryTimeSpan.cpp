#include "MutexLock.h"
#include "UDPRetryTimeSpan.h"

using namespace std;

UDPRetryTimeSpan* UDPRetryTimeSpan::_instance = NULL;
pthread_mutex_t UDPRetryTimeSpan::_mutex = PTHREAD_MUTEX_INITIALIZER;

UDPRetryTimeSpan::UDPRetryTimeSpan() {
    // retriedCnt, waitTimeUs
    ZERO = make_tuple<int, int>(0, 0);
    ONE = make_tuple<int, int>(1, 10 * 1000);
    TWO = make_tuple<int, int>(2, 50 * 1000);
    THREE = make_tuple<int, int>(3, 100 * 1000);
    FOUR = make_tuple<int, int>(4, 500 * 1000);
    FIVE = make_tuple<int, int>(5, 1000 * 1000);
    SIX = make_tuple<int, int>(6, 2000 * 1000);

    vec.push_back(ZERO);
    vec.push_back(ONE);
    vec.push_back(TWO);
    vec.push_back(THREE);
    vec.push_back(FOUR);
    vec.push_back(FIVE);
    vec.push_back(SIX);
}

UDPRetryTimeSpan* UDPRetryTimeSpan::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex);
        if (_instance == NULL) {
            _instance = new UDPRetryTimeSpan();
        }
    }

    return _instance;
}

int UDPRetryTimeSpan::maxTryCnt() {
    return vec.size() - 1;
}

int UDPRetryTimeSpan::allWaitTimeUs() {
    int sum = 0;
    for (vector<tuple<int, int>>::iterator it = vec.begin(); it != vec.end(); it++) {
        sum += get<1>(*it);
    }

    return sum;
}

int UDPRetryTimeSpan::waitTimeUs(int triedCnt) {
    if (triedCnt < 0 || triedCnt >= (int)(vec.size())) {
        return DEFAULT_TIME_SPAN_US;
    }

    return get<1>(vec[triedCnt]);
}
