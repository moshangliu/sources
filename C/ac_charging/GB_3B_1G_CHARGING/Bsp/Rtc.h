#ifndef __RTC_H__
#define __RTC_H__

#include "lpc177x_8x_rtc.h"


extern RTC_TIME_Type RtcCurTime;

extern RTC_TIME_Type RtcGetTime ( void );                                // ��ȡRTCʱ��
extern void RtcSetTime ( RTC_TIME_Type time );                           // ����RTCʱ��
extern void RtcInit ( void );                                             // ��ʼ��ʱ��

#endif

