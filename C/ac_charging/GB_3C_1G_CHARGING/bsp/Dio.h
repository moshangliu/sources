#ifndef __DIO_H__
#define __DIO_H__
#define INPUT_NUM_MAX 4 //����


#define ON 1
#define OFF 0

#define INPUT1 1
#define INPUT2 2
#define INPUT3 3
#define INPUT4 4

#if CPU822
#define OUTPUT_NUM_MAX 6 //
#define RELAY1 1
#define RELAY2 2
#define RELAY3 3
#define RELAY4 4

#define LED1   5
#define LED2   6
#else
#define OUTPUT_NUM_MAX 7 //
#define RELAY1 1
#define RELAY2 2
#define RELAY3 3
#define RELAY4 4
#define RELAY5 5
#define LED1   6
#define LED2   7
#endif

void InputCheck ( void );
uint8_t CheckDoorSta(void);
uint8_t CheckEStopSta(void);
void DioInit ( void );                                         // ��ʼ���˿�����
uint8_t GetInputStatus ( uint8_t num );                      // ��ȡ����˿�״̬
uint8_t GetAddress ( void );                              // ����ַ���أ����ص�ַ
void SetOutput ( uint8_t num );  // �������״̬ num : ����˿ں� 1-6  LED 10-12
void ClrOutput ( uint8_t num );  // ������״̬ num : ����˿ں� 1-6  LED 10-12

void OutEnable(void);
void OutDisable(void);

#endif