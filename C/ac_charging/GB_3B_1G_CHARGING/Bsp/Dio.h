#ifndef __DIO_H__
#define __DIO_H__
#define INPUT_NUM_MAX       2 //����
#define OUTPUT_NUM_MAX      7
#define ADDR_NUM_MAX        6
#define LED_NUM_MAX         2
#define GPRS_NUM_MAX        2
#define PWMCP_NUM_MAX       1
#define ON 1
#define OFF 0

#define INPUT1 1
#define INPUT2 2

#define RELAY1 1
#define RELAY2 2
#define RELAY3 3
#define RELAY4 4
#define RELAY5 5
#define RELAY6 6

#define LED1   10
#define LED2   11

#define PWMCP  20

void InputCheck ( void );
void DioInit ( void );                                         // ��ʼ���˿�����
uint8_t GetInputStatus ( uint8_t num );                      // ��ȡ����˿�״̬
void SetOutput ( uint8_t num );  // �������״̬ num : ����˿ں� 1-4  LED 10-11
void ClrOutput ( uint8_t num );  // ������״̬ num : ����˿ں� 1-4  LED 10-11
void FlipOutput (uint8_t num);
void led_run ( uint8_t num );
#endif