
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

#include "drv_inc.h"
#include "Bsp_inc.h"
#include "app_inc.h"

uint32_t ConnectorCheckCnt;             //������״̬��������
uint8_t ConnectorStatus;             //������״̬
uint8_t ConnectorCnt;             //������״̬5��ѭ������
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

void Charging_main( void )
{
 //   if (Dev_Status == 1)
 //   {
        //����������ʱ���ֵ�������״̬ż���仯������������ַ������в���
        //��ʽ1��1s���������״̬
        /*
        if(( SysTickCnt - ConnectorCheckCnt ) > 1000 )
        {
            g_sAnalogValue.linkv = GetAnalogSta();
            ConnectorCheckCnt = SysTickCnt;
        } 
        */  
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
                    g_sAnalogValue.linkv = ConnectorStatus;
                    ConnectorCnt = 6;
                }
            }
        }     
   // }
/*    
    if (Dev_Status != 1)
    {
        if(( SysTickCnt - QRCodeCnt ) > 17000 )            //15�뷢��һ�ζ�ά��ɨ���ѯ
        {
            QR_code_Verification();
            QRCodeCnt = SysTickCnt;
        } 
    }
*/    
    if (LoginPermission == 1)   //���ܵ�ƽ̨�����û���Ϣ����������,��ǰƽ̨3�뷢һ�����ȷ��
    {
        LoginPermission = 0;
        if( g_sAnalogValue.linkv == 6 )            
        {
            ChargingPermission = 0;
            begin_CurKwh = CurKwh; 
            Start();
        } 
    }
        
    if(ChargingPermission == 1)                 //��ú�̨�����ɺ��������
    {
        record_clear();
        ChargingPermission = 0;
        OUTPUTOPEN();               //���ܵ�ƽ̨���������Ķ�������ʽ�������
        Dev_Status = 1;                 //�豸״̬Ϊ���
        RUNLEDOPEN();
        Cur_0_BeginTime = SysTickCnt;
    }
    
    if (StopRequest == 1)        //���ܵ�ƽ̨�����û���Ϣ����������,��ǰƽ̨5�뷢һ�����ȷ��
    {
        StopRequest = 0;
        end_CurKwh = CurKwh;
        Dev_Status = 0;
        OUTPUTCLOSE();//ֹͣ���
        RUNLEDCLOSE();
        Stop();
        StopRequestCnt = SysTickCnt;
    }
    
    if(StopPermission == 1)
    {
        StopPermission = 0;
    }
}





