#include "Common.h"
#include "UDPSendDataStructs.h"
#include "UDPRetryTimeSpan.h"
#include "MutexLock.h"

using namespace std;

UDPResendObj::UDPResendObj(UDPFrame* frame)
    : _frame(frame), _triedCnt(0), _sendTsUs(current_us()) {}

UDPResendObj::~UDPResendObj() {
    delete _frame;
}

void UDPResendObj::update() {
    _triedCnt++;
    _sendTsUs += UDPRetryTimeSpan::instance()->waitTimeUs(_triedCnt);
}

UDPResendQueue* UDPResendQueue::_instance = NULL;
pthread_mutex_t UDPResendQueue::_mutex = PTHREAD_MUTEX_INITIALIZER;

UDPResendQueue::UDPResendQueue() {}
UDPResendQueue* UDPResendQueue::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex);
        if (_instance == NULL) {
            _instance = new UDPResendQueue();
        }
    }

    return _instance;
}
