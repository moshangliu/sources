#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "Meter.h"
#include "GPRS.h"
#include "Charging.h"

#define SPOTOUTPUT  0x01
#define SPOTGREEN   0x02
#define SPOTYELLOW  0x03
#define SPOTRESVED  0x04

#define LPC822 0


/*
���ò������ݽṹ
*/
typedef struct
{
    uint8_t   id[4];                                               // ���׮��ַ
    uint16_t   price;                                                    // ���
} PARACONFIG;

#pragma pack()
//------------------------------------------------------------------------------
#define OUTPUTCLOSE ClrOutput(5);
#define RUNLEDCLOSE ClrOutput(2);
#define OUTPUTOPEN SetOutput(5);
#define RUNLEDOPEN SetOutput(2);


#define   SWAP(a)           ((a)>>8 | (a)<<8)

extern volatile unsigned long SysTickCnt;
extern PARACONFIG g_sParaConfig;

#endif

