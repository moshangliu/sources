
/**********************************************************
�º�����Դ�Ƽ����޹�˾
File name:  hwtst.c	              // �ļ���
Author:     ���һ�                   // ����
Version:    v1.00                   // �汾��Ϣ
Date:       2015-2-27              // �������
Description:  	              // 
Others:		              // 
Function List:	              //
History:                        //
**********************************************************/


#include "board.h"
#include "Dio.h"
#include "uart.h"
#include "globals.h"
#include "GPRS.h"
#include "string.h"

extern uint8_t GetAnalogSta(void);
uint32_t ConnectorCheckCnt;             //������״̬��������
uint8_t ConnectorStatus;             //������״̬
uint8_t ConnectorCnt;             //������״̬5��ѭ������
uint8_t linkv;                  //����������ѹ
extern PARACONFIG g_sParaConfig;
extern uint8_t Dev_Status;
uint32_t QRCodeCnt;         //��ά��ɨ�������ʱ
uint8_t LoginPermission = 0;       //��¼���
uint32_t ChargingRequestCnt;         //��������ʱ
uint8_t ChargingPermission;       //������
uint8_t StopRequest;            //��ֹ̨ͣ����
uint32_t StopRequestCnt;           //ֹͣ�����ʱ
uint8_t StopPermission;       //ֹͣ���
extern uint32_t Cur_0_BeginTime,Cur_0_Time;
extern uint32_t begin_CurKwh, end_CurKwh;
extern uint32_t CurKwh;
extern uint8_t EStopFlag,ParaError,MeterCommErr;
uint8_t test;
void Charging_main( void )
{
  //��ʽ2��200ms���������״̬������5�ν����ͬ����Ϊ��Чֵ
  
  if(( SysTickCnt - ConnectorCheckCnt ) > 200 )
  {
    ConnectorCheckCnt = SysTickCnt;
    if (ConnectorStatus != GetAnalogSta())
    {
      ConnectorStatus = GetAnalogSta();
      ConnectorCnt = 1;
    }
    else 
    {
      ConnectorCnt += 1;
      if (ConnectorCnt > 5)
      {
        linkv = ConnectorStatus;
        ConnectorCnt = 6;
      }
    }
  }     
 
  if ((LoginPermission == 1))   //���ܵ�ƽ̨�����û���Ϣ����������,��ǰƽ̨3�뷢һ�����ȷ��
  {
    LoginPermission = 0;
    if (Dev_Status==0)
    {
      if( linkv == 6 )            
      {
        ChargingPermission = 0;
        begin_CurKwh = CurKwh; 
        Start();
      } 
    }
  }
  
  if(ChargingPermission == 1)                 //��ú�̨�����ɺ��������
  {
    record_clear();
    ChargingPermission = 0;
    //    SetOutput(5);           //����
    OUTPUTOPEN ;
    SetOutput(2);           //����ָʾ��
    Dev_Status = 1;                 //�豸״̬Ϊ���
    Cur_0_BeginTime = SysTickCnt;
  }
  
  if (StopRequest == 1)        //���ܵ�ƽ̨�����û���Ϣ����������,��ǰƽ̨5�뷢һ�����ȷ��
  {
    StopRequest = 0;
    end_CurKwh = CurKwh;
    Dev_Status = 0;
    //       ClrOutput(1);           //ֹͣ���
    OUTPUTCLOSE;
    ClrOutput(2);           //�ر�����ָʾ��
    Stop();
    //      StopRequestCnt = SysTickCnt;
  }
  
  if(StopPermission == 1)
  {
    StopPermission = 0;
  }
  
  
  if( GetInputStatus(1)&&Dev_Status )
  {
    end_CurKwh = CurKwh;
    Stop();
    Dev_Status = 0;
  }
  if (GetInputStatus(1))
  {
    ClrOutput(2);
    EStopFlag = 1;
    OUTPUTCLOSE;
  }
  else
  {
    EStopFlag = 0;
  }   
  
  if (MeterCommErr||ParaError||EStopFlag)
  {
     SetOutput(3);
  }
  else
  {
     ClrOutput(3);
  }
}





