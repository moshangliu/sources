
#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_pinsel.h"

#include "Dio.h"

const uint8_t InputConfig[INPUT_NUM_MAX][2]=
{
    4,5,
    2,12,
};

const uint8_t OutputConfig[OUTPUT_NUM_MAX][2]=
{
    4,4,
    2,13,
    0,11,
    0,10,
    4,3,
    0,1,
    3,5,
};

const uint8_t LedConfig[LED_NUM_MAX][2]=
{
    3,24,
    3,25,
};

const uint8_t GPRSConfig[GPRS_NUM_MAX][2]=
{
    3,3,
    5,0,
};

const uint8_t PWMCPConfig[PWMCP_NUM_MAX][2]=
{
    3,5,
};

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
    if ( ( num > 0 ) || ( num < INPUT_NUM_MAX ) )
    {
        return InputSta[num-1];
    }
    else
    {
        return OFF ;
    }
}

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
    uint32_t PortValue;
    uint8_t i;
    for (i=0;i<INPUT_NUM_MAX;i++)
    {
        PortValue = GPIO_ReadValue ( InputConfig[i][0] );
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
  Input:       num : ����˿ں� 1-9  LED 10-12  pwmcp 20
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void SetOutput ( uint8_t num )
{
    if (num <= OUTPUT_NUM_MAX)
    {
        GPIO_OutputValue ( OutputConfig[num-RELAY1][0], (1<<OutputConfig[num-RELAY1][1]), ON );
    }
    else if(num >= LED1 && num < LED_NUM_MAX+LED1 )
    {
         GPIO_OutputValue ( LedConfig[num-LED1][0], (1<<LedConfig[num-LED1][1]), ON );
    }
    else if(num == PWMCP)
    {
         GPIO_OutputValue ( PWMCPConfig[num-PWMCP][0], (1<<PWMCPConfig[num-PWMCP][1]), ON );
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
    if (num < OUTPUT_NUM_MAX)
    {
        GPIO_OutputValue ( OutputConfig[num-RELAY1][0], (1<<OutputConfig[num-RELAY1][1]), OFF );
    }
    else if(num >= LED1 && num < LED_NUM_MAX+LED1 )
    {
        GPIO_OutputValue ( LedConfig[num-LED1][0], (1<<LedConfig[num-LED1][1]), OFF );
    }
    else if(num == PWMCP)
    {
         GPIO_OutputValue ( PWMCPConfig[num-PWMCP][0], (1<<PWMCPConfig[num-PWMCP][1]), OFF );
    }
    
}

void FlipOutput (uint8_t num)
{
    uint32_t tmp;
    if (num < OUTPUT_NUM_MAX)
    {
        tmp = GPIO_ReadValue (OutputConfig[num-RELAY1][0]);
        if(tmp&(1<<OutputConfig[num-RELAY1][1]))
        {
            ClrOutput(num); 
        }
        else
        {
            SetOutput(num);
        }
    }
    else if(num >= LED1 && num < LED_NUM_MAX+LED1 )
    {
        tmp = GPIO_ReadValue (LedConfig[num-LED1][0]);
        if(tmp&(1<<LedConfig[num-LED1][1]))
        {
            ClrOutput(num); 
        }
        else
        {
            SetOutput(num);
        }
    }
    else if(num == PWMCP)
    {
        tmp = GPIO_ReadValue (PWMCPConfig[num-PWMCP][0]);
        if(tmp&(1<<PWMCPConfig[num-PWMCP][1]))
        {
            ClrOutput(num); 
        }
        else
        {
            SetOutput(num);
        }
    }
    
}
/*************************************************************
  Function: void InitDio(void)
  Description: ��ʼ���˿�����
  Calls:       GPIO_SetDir
  Called By:   main
  Input:       ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void DioInit ( void )
{
    uint8_t i=0;
    
    for (i=0;i<INPUT_NUM_MAX;i++)
    {
        PINSEL_ConfigPin ( InputConfig[i][0], (1<<InputConfig[i][1]), 0 );//������IO��������ΪGPIO����
        GPIO_SetDir ( InputConfig[i][0], (1<<InputConfig[i][1]), GPIO_DIRECTION_INPUT );//������IO��������Ϊ����
        PINSEL_SetPinMode ( InputConfig[i][0], (1<<InputConfig[i][1]), IOCON_MODE_PULLUP );//IO��������Ϊ�ڲ�����
    }
    
    for (i=0;i<OUTPUT_NUM_MAX;i++)
    {
        PINSEL_ConfigPin ( OutputConfig[i][0], (1<<OutputConfig[i][1]), 0 );
        GPIO_OutputValue ( OutputConfig[i][0], (1<<OutputConfig[i][1]), OFF );
        GPIO_SetDir ( OutputConfig[i][0], (1<<OutputConfig[i][1]), GPIO_DIRECTION_OUTPUT );
        PINSEL_SetPinMode ( OutputConfig[i][0], (1<<OutputConfig[i][1]), IOCON_MODE_PULLUP );//IO��������Ϊ�ڲ�����
    }

    for (i=0;i<LED_NUM_MAX;i++)
    {
        PINSEL_ConfigPin ( LedConfig[i][0], (1<<LedConfig[i][1]), 0 );
        GPIO_SetDir ( LedConfig[i][0], (1<<LedConfig[i][1]), GPIO_DIRECTION_OUTPUT );
        PINSEL_SetPinMode ( LedConfig[i][0], (1<<LedConfig[i][1]), IOCON_MODE_PULLUP );//IO��������Ϊ�ڲ�����
    }
    
    for (i=0;i<PWMCP_NUM_MAX;i++)
    {
        PINSEL_ConfigPin ( PWMCPConfig[i][0], (1<<PWMCPConfig[i][1]), 0 );
        GPIO_SetDir ( PWMCPConfig[i][0], (1<<PWMCPConfig[i][1]), GPIO_DIRECTION_OUTPUT );
        PINSEL_SetPinMode ( PWMCPConfig[i][0], (1<<PWMCPConfig[i][1]), IOCON_MODE_PULLUP );//IO��������Ϊ�ڲ�����
    }
}