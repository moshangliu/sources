/**********************************************************
  ���յº�����Դ�Ƽ����޹�˾
  File name:  Dio.c	              // �ļ���
  Author:     ����                    // ����
  Version:    v1.00                   // �汾��Ϣ
  Date:       2012-8-15               // �������
  Description:  	              // �������롢�����������������
  Others:		              // ��
  Function List:	              // void DioInit(void) ��ʼ���˿�����
                                         uint8_t GetInputStatus(uint8_t num)  ��ȡ����˿�״̬
                                         uint8_t AddrCheck(void) ����ַ���أ����ص�ַ
                                         void SetOutput(uint8_t num) �������״̬ num : ����˿ں� 1-6  LED 10-12
                                         void ClrOutput(uint8_t num) ������״̬ num : ����˿ں� 1-6  LED 10-12
  History:2013-05-31 ���ö�ά�������ԭ�궨�巽ʽ����IO�˿�,���Ķ˿ڸ�����
**********************************************************/


#include "board.h"
#include "Dio.h"

#if CPU822
const uint8_t InputConfig[INPUT_NUM_MAX][2]=
{
    0,26,
    0,27,
    0,16,
    0,10
};


const uint8_t OutputConfig[OUTPUT_NUM_MAX][2]=
{
    0,1,
    0,15,
    0,24,
    0,25,
    0,6,
    0,7  
};

#else
const uint8_t InputConfig[INPUT_NUM_MAX][2]=
{
    0,27,
    0,16,
    0,10,
    0,11
};


const uint8_t OutputConfig[OUTPUT_NUM_MAX][2]=
{
    0,1,
    0,15,
    0,24,
    0,25,
    0,26,
    0,6,
    0,7  
};
#endif

#define RUNLED 6 //���е�
#define WARLED 7 //������
#define INSULATIONEN 9 //��Ե������
#define FAN_CTRL 1 //����
#define OUTRELAY 2 //���Ӵ���
#define DISCHARGE 3 //й�ŽӴ���
#define BMSPOWERPORT 4 //����Դ+
#define BMSPOWERPORT2 10 //����Դ-
#define LOCKRELAY 5 //��
#define UNLOCKRELAY 8 //����
#define WD_CONNECTED_PORT		(5)
#define WD_CONNECTED_PIN		(1)
#define WD_CONNECTED_MASK		(1 << WD_CONNECTED_PIN)

uint8_t InputSta[INPUT_NUM_MAX];
static uint8_t InputStaCur[INPUT_NUM_MAX];
static uint8_t InputStaPre[INPUT_NUM_MAX];
static uint8_t InputStaCnt[INPUT_NUM_MAX];

/*************************************************************
  Function: GetInputStatus(uint8_t num)
  Description: ��ȡ����˿�״̬
  Calls:       ��
  Called By:   ��
  Input:       num : ����ͨ���ţ���Χ1-2
  Output:      ��
  Return:      ����˿�״̬
  Others:      ��
*************************************************************/
uint8_t GetInputStatus ( uint8_t num )
{
    if ( ( num > 0 ) && ( num <= INPUT_NUM_MAX ) )
    {
        return InputSta[num-1];
    }
    else
    {
        return OFF ;
    }
}
    uint32_t PortValue;
/*************************************************************
  Function: void InputCheck(void)
  Description: ������ܽ�״̬����������󣬽�״̬ˢ�µ�����
  Calls:       ��
  Called By:   SysTick_Handler
  Input:       ��
  Output:      InputSta[]
  Return:      ��
  Others:      ��
*************************************************************/
void InputCheck ( void )
{

    uint8_t i;
    for (i=0;i<INPUT_NUM_MAX;i++)
    {
        PortValue = Chip_GPIO_GetPortValue(LPC_GPIO_PORT, 0);
        if (PortValue & (1<<InputConfig[i][1]))
        {
            InputStaCur[i] = OFF ;
        }
        else
        {
            InputStaCur[i] = ON ;
        }        
    }

    for ( i = 0; i < INPUT_NUM_MAX; i++ )
    {
        if ( InputStaCur[i] == InputStaPre[i] )
        {
            InputStaCnt[i]++;

            if ( InputStaCnt[i] > 10 )
            {
                InputSta[i] = InputStaCur[i] ;
            }
        }
        else
        {
            InputStaCnt[i] = 0 ;
        }

        InputStaPre[i] = InputStaCur[i] ;
    }
}



/*************************************************************
  Function: void SetOutput(uint8_t num)
  Description: ��������˿�״̬
  Calls:       GPIO_OutputValue
  Called By:   SysTick_Handler��
  Input:       num : ����˿ں� 1-9  LED 10-12
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void SetOutput ( uint8_t num )
{
     if ( ( num > 0 ) && ( num <= OUTPUT_NUM_MAX ) )
     {
        Chip_GPIO_SetPinState ( LPC_GPIO_PORT, OutputConfig[num-1][0] ,OutputConfig[num-1][1], ON );
     }
}

/*************************************************************
  Function: void ClrOutput(uint8_t num)
  Description: �������˿�״̬
  Calls:       GPIO_OutputValue
  Called By:   SysTick_Handler��
  Input:       num : ����˿ں� 1-6
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void ClrOutput ( uint8_t num )
{
     if ( ( num > 0 ) && ( num <= OUTPUT_NUM_MAX ) )
     {
        Chip_GPIO_SetPinState ( LPC_GPIO_PORT, OutputConfig[num-1][0] ,OutputConfig[num-1][1], OFF );
     }
}

/*************************************************************
  Function: void OutEnable(void)
  Description: ��������Ӵ���
  Calls:       SetOutput()
  Called By:
  Input:       sts--ָ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void OutEnable(void)
{
    SetOutput(OUTRELAY);
    ClrOutput(INSULATIONEN);//�رվ�Եģ��
}
/*************************************************************
  Function: void OutDisable(void)
  Description: �Ͽ�����Ӵ���
  Calls:       ClrOutput()
  Called By:
  Input:       
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void OutDisable(void)
{
    ClrOutput(OUTRELAY);
    SetOutput(INSULATIONEN);//��Ե����
}

