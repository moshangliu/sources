
#include "globals.h"
#include "Bsp_inc.h"
#include "app_inc.h"
#include "drv_inc.h"

#define LED1 10

/* EXTERNALS */
extern void peer_main ( void );
extern void CanErr ( void );
/* GLOBALS */
volatile unsigned long  SysTickCnt;                            /* counts 1ms timeTicks */
uint8_t AcContactor;
uint32_t g_lDevID;
PARACONFIG   g_sParaConfig;
RTC_TIME_Type CurTime ;

const PARACONFIG  g_sParaConfig_init =
{
   {1,0,0,0},
   100
};

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler ( void )
{
    SysTickCnt++;
    /*
    if ( ! ( SysTickCnt % 10 ) )
    {
       FlipOutput(LED1);
    }
    */
    //������˿�
    InputCheck();
    //����CAN������
    CAN_Send_main ();
    //�ж�UART����֡�Ƿ����
    FrameIsEnd();      
    CanErr();
  
    KeyBoard_Scan();
}

/*************************************************************
Function: void SysTick_Handler(void)
Description: SYSTICK�жϺ���
Calls:
Called By:   main()
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void TIMER0_IRQHandler ( void )
{
    static uint16_t tmpcnt; 
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)== SET)
    {
       if( ++tmpcnt >= 100)
       {
           tmpcnt = 0;
           SetOutput(PWMCP);
       }
       else if( tmpcnt == 25 )
       {
           ClrOutput( PWMCP );
       }
    }
    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}

/*************************************************************
Function: void InitSysTick(void)
Description: SYSTICK��ʼ������
Calls:
Called By:   main()
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void SysTickInit ( void )
{
    //Initialize System Tick with 1ms time interval
    SYSTICK_InternalInit ( 1 );
    
    //Enable System Tick interrupt
    SYSTICK_IntCmd ( ENABLE );
    
    //Enable System Tick Counter
    SYSTICK_Cmd ( ENABLE );
}


void Timer0Init(void)
{
    TIM_TIMERCFG_Type TIM_ConfigStruct;
    TIM_MATCHCFG_Type TIM_MatchConfigStruct ;    
    // Initialize timer 0, prescale count time of 100uS
    TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
    TIM_ConfigStruct.PrescaleValue	= 5;
    
    // use channel 0, MR0
    TIM_MatchConfigStruct.MatchChannel = 0;
    // Enable interrupt when MR0 matches the value in TC register
    TIM_MatchConfigStruct.IntOnMatch   = TRUE;
    //Enable reset on MR0: TIMER will reset if MR0 matches it
    TIM_MatchConfigStruct.ResetOnMatch = TRUE;
    //Stop on MR0 if MR0 matches it
    TIM_MatchConfigStruct.StopOnMatch  = FALSE;
    //Toggle MR0.0 pin if MR0 matches it
    TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
    // Set Match value, count value of 10000 (10000 * 100uS = 1000000us = 1s --> 1 Hz)
    TIM_MatchConfigStruct.MatchValue   = 1;//10*100us=1ms
    
    // Set configuration for Tim_config and Tim_MatchConfig
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
    TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);
    
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
    /* Enable interrupt for timer 0 */
    NVIC_EnableIRQ(TIMER0_IRQn);
    // To start timer 0
    TIM_Cmd(LPC_TIM0,ENABLE);    
}
/*************************************************************
Function: void Delay ( unsigned long tick )
Description: ��ʱ��������ʱtick
Calls:
Called By:
Input:       tick
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void Delay ( unsigned long tick )
{
    unsigned long s;
    
    s = SysTickCnt;
    
    while ( ( SysTickCnt - s ) < tick );
}




/*************************************************************
Function: void LoadAllParameters(void)
Description: ������ϵͳ������Ҫ�Ĳ���
Calls:
Called By:   main()
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void LoadAllParameters ( void )
{
    uint16_t rpt;
    for ( rpt = 0; rpt < 3; rpt++ )
    {
        if ( LoadParameter() == 0 )
        {
            break;
        }
    }
    
    if ( rpt >= 3 )
    {
        uint8_t *pdata = ( uint8_t * ) &g_sParaConfig_init;
        memcpy ( ( uint8_t * ) &g_sParaConfig, pdata, sizeof ( g_sParaConfig ) );
        
        SaveParameter();
    }
}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
* @brief        c_entry: Main program body
* @param[in]	None
* @return       int
**********************************************************************/
void c_entry ( void )
{
    __disable_irq();
    
    DioInit();                                                        //��ʼ��IO
    RtcInit();                                                      //��ʼ��ʱ��
    EEPROM_Init();                                                //��ʼ��EEPROM
    CanInit();                                                   //��ʼ��CAN�˿�
    UartsInit();                                                  // ��ʼ��RS232
    AdcInit();                                                      // ��ʼ��ADC
    LoadAllParameters();                                         // ��ȡ���в���
    SysTickInit();                                             // ��ʼ��TICK�ж�
    Timer0Init();    
    LCDInit();
    Disp_TCB_Init();
//    g_lDevID = GetAddress();
    
    __enable_irq();
    
    
}


int main( void )
{
    /* Configure the hardware for use by this demo. */
    c_entry();

    for( ;; )
    {
          RtcGetTime(); 
          peer_main();
          MeterDeal();
          adc_main();
     //     HwMain();
     //     DisplayMain();
          GPRS_main();
          Charging_main();
          
          
          if((SysTickCnt%500) == 0)
          {
             CurTime = RtcGetTime();
          }
          can_test();
    }
}
/*-----------------------------------------------------------*/

