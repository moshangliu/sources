#ifndef __AD_H__
#define __AD_H__

#include <string.h>

uint32_t GetCurrent ( uint8_t num );                          // ��ȡ��ǰ����ֵ
uint32_t GetFuseSta ( uint8_t num );                        // ��ȡ��ǰ��˿״̬
uint32_t GetTemp ( uint8_t num );                               // ��ȡ��ǰ�¶�

extern void AdcInit ( void );
extern void adc_main ( void );

#endif