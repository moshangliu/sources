

/**********************************************************
�º�����Դ�Ƽ����޹�˾
File name:  hwtst.c	              // �ļ���
Author:     ���һ�                   // ����
Version:    v1.00                   // �汾��Ϣ
Date:       2015-1-23              // �������
Description:  	              // ʹ��232������GPRSģ��ͨ�ţ����ʵ�����ۺ�ƽ̨��ͨ��
Others:		              // 
Function List:	              //
History:                        //
**********************************************************/

#include "lpc177x_8x_crc.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_gpdma.h"
#include "lpc177x_8x_pinsel.h"
#include "Bsp_inc.h"
#include "app_inc.h"

//У��������
extern UART_BUFF RS485Rx2, RS485Tx2;
uint8_t MeterData[4], MeterAddr[6];
uint8_t MeterErr, MeterCommErr, MeterAddrFlag;
uint8_t Position68;
uint32_t RdMeterTicks, RdMeterTicks1, RdMeterTicks2;       //������������ѹ��������ʱ������
uint8_t Meter_counter;
uint16_t MeterOnlineCnt;
uint32_t CurKwh;
uint32_t Cur_0_BeginTime,Cur_0_Time;
uint8_t RelaySta2;      //4852����״ָ̬ʾ�ƣ����濪��
extern uint16_t output_vol, output_cur;         //�����ѹ������
                                                //�����������λС�����������ر���������һλ��ϵͳ�а�����λС������
                                                //��ѹ�����һλС�����������ر���С���ڶ�λ��0��ϵͳ�а�����λС������


/*************************************************************
Function: uint32_t GetCurKwh ( void )
Description: ��ȡ��ǰ����
Calls:       ��
Called By:   
Input:       
Output:      ��
Return:      CurKwh ��ǰ�����й��ܵ���
Others:      ��
*************************************************************/
uint32_t GetCurKwh ( void )
{
    return CurKwh;
}
/*************************************************************
Function: uint8_t CSCalc(UART_BUFF f)
Description: �����վ����Ӧ���У����
Calls:       ��
Called By:   void MeterGetRs232();void MeterCalc(void);
Input:       UART_BUFF f Ӧ��֡����
Output:      ��
Return:      uint8_t cs У����
Others:      ��
*************************************************************/
uint8_t CSCalc ( UART_BUFF f )
{
    uint8_t cs = 0x00, i, len, j;
    len = f.Len;
    Position68 = 0 ;

    for ( i = 0; i < len; i++ )
    {
        if ( f.Buff[i] == 0x68 )
        {
            Position68 = i ;

            for ( j = i; j < len - 2; j++ )
            {
                cs += f.Buff[j];
            }

            break;
        }
    }

    return cs;
}

/*************************************************************
Function: void MeterDeal(void)
Description: ��ȡ�����
Calls:       ��
Called By:
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void ReadMeterAddr ( void ) //��ȡ�����
{

}
/*************************************************************
Function: void BcdToHex(uint32_t *dat ,uint8_t *ptr)
Description: BCD����ת��Ϊ16����
Calls:       ��
Called By:   
Input:       *dat--HEX����ָ�� *ptr--BCD����ָ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void BcdToHex(uint32_t *dat ,uint8_t *ptr)
{
    uint32_t tmp ;
    uint8_t tmp1;
    tmp = 0 ;
    tmp1 = *ptr ;
    tmp = (tmp1>>4)*10 ;
    tmp1 &= 0x0f;
    tmp = tmp + tmp1 ;
    
    tmp1 = *(ptr+1);
    tmp += (tmp1>>4)*1000 ;
    tmp1 &= 0x0f ;
    tmp += tmp1*100;
    
    tmp1 = *(ptr+2);
    tmp += (tmp1>>4)*100000 ;
    tmp1 &=0x0f ;
    tmp += tmp1*10000 ;
    
    tmp1 = *(ptr+3);
    tmp += (tmp1>>4)*10000000 ;
    tmp1 &= 0x0f;
    tmp += tmp1*1000000 ;
    *dat = tmp ;    
}
/*************************************************************
Function: void MeterGetRs232()
Description: ��վӦ��֡
Calls:       uint8_t CSCalc(UART_BUFF f)
Called By:   ��
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void GetMeterMsg ( void ) //��վӦ��֡
{

    uint8_t  len, i, j, CS;
    uint8_t tmp_[4];
    uint8_t temp;
    uint32_t tmp,tmp1,tmp2;
    if ( GetRs485Ch2Sta() == 1 ) //֡���ս������
    {
        //����״ָ̬ʾ��
        RelaySta2 = ~ RelaySta2 ;
        temp = RelaySta2;
        if ( temp & 0x1 )
        {
            SetOutput ( LED2 );
        }
        else
        {
            ClrOutput ( LED2 );
        }
        //
        MeterOnlineCnt = 1;
        len = RS485Rx2.Len;
        CS = CSCalc ( RS485Rx2 );
        RS485Rx2.Flag = 0 ;
        RS485Rx2.Len = 0 ;
        RS485Rx2.Idx = 0 ;        

        if ( CS == RS485Rx2.Buff[len-2] )
        {
            if ( RS485Rx2.Buff[8+Position68] == 0x91 ) //��ȡ�����й��ܵ���,����Ӧ��
            {
                switch (RS485Rx2.Buff[9 + Position68])
                {
                case 0x06:
                    {
                        output_vol = 0;
                        for ( i = 14 + Position68, j = 0; j < 2; i++, j++ )
                        {
                            tmp_[j] = RS485Rx2.Buff[i] - 0x33;
                        }
                        tmp = ((tmp_[0] >> 4) & 0x0f) * 100 + (tmp_[0] & 0x0f) *10;
                        tmp1 = ((tmp_[1] >> 4) & 0x0f) * 10000 + (tmp_[1] & 0x0f) *1000;
                        output_vol = tmp + tmp1 ;
                        break;
                    }
                case 0x07:
                    {
                        output_cur = 0;
                        for ( i = 14 + Position68, j = 0; j < 3; i++, j++ )
                        {
                            tmp_[j] = RS485Rx2.Buff[i] - 0x33;
                        }
                        tmp = ((tmp_[0] >> 4) & 0x0f) ;
                        tmp1 = ((tmp_[1] >> 4) & 0x0f) * 100 + (tmp_[1] & 0x0f) *10;
                        tmp2 = ((tmp_[2] >> 4) & 0x0f) * 10000 + (tmp_[2] & 0x0f) *1000;
                        output_cur = tmp + tmp1 + tmp2; 
                        if (output_cur > 100)           //��������1A
                        {
                           Cur_0_BeginTime = SysTickCnt; 
                        }
                        break;
                    }
                case 0x08://���������
                    {
                        for ( i = 14 + Position68, j = 0; j < 4; i++, j++ )
                        {
                            MeterData[j] = RS485Rx2.Buff[i] - 0x33;
                        }
                        BcdToHex ( &CurKwh, MeterData ); //�����BCD��,תΪHEX    
                        break;
                    }
                }
            }
            else if ( RS485Rx2.Buff[8+Position68] == 0xD1 ) //�����������վ�쳣Ӧ��
            {
                MeterErr = RS485Rx2.Buff[len-3]; //��ȡ������Ϣ��
            }
        }
    }

}

/*************************************************************
Function: void MeterCalc(void)
Description: ��ȡ�����й��ܵ���
Calls:       uint8_t CSCalc(UART_BUFF f)
Called By:   ��
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void MeterCalc ( void ) //��ȡ�����й��ܵ��� 68 AA AA AA AA AA AA 68 11 04 33 33 34 33 AE 16
{

    uint8_t CS;
    RS485Tx2.Buff[0] = 0xFE; //ǰ���ֽ�
    RS485Tx2.Buff[1] = 0xFE;
    RS485Tx2.Buff[2] = 0xFE;
    RS485Tx2.Buff[3] = 0xFE;
    RS485Tx2.Buff[4] = 0x68; //֡��ʼ��

    RS485Tx2.Buff[5] = 0xaa;//MeterAddr[0];//��ַ��
    RS485Tx2.Buff[6] = 0xaa;//MeterAddr[1];
    RS485Tx2.Buff[7] = 0xaa;//MeterAddr[2];
    RS485Tx2.Buff[8] = 0xaa;//MeterAddr[3];
    RS485Tx2.Buff[9] = 0xaa;//MeterAddr[4];
    RS485Tx2.Buff[10] = 0xaa;//MeterAddr[5];

    RS485Tx2.Buff[11] = 0x68; //֡��ʼ��
    RS485Tx2.Buff[12] = 0x11; //������
    RS485Tx2.Buff[13] = 0x04; //�����򳤶�
    RS485Tx2.Buff[14] = 0x33; //���ݱ�ʶ��
    RS485Tx2.Buff[15] = 0x33;
    RS485Tx2.Buff[16] = 0x34;
    RS485Tx2.Buff[17] = 0x33;

    RS485Tx2.Len = 20;//���ͳ���
    CS = CSCalc ( RS485Tx2 );
    RS485Tx2.Buff[18] = CS; //У����
    RS485Tx2.Buff[19] = 0x16; //������

    RS485Ch2SendMsg();//��ȡ����ܵ�������֡

}

/*************************************************************
Function: void MeterVol(void)
Description: ����ѹ
Calls:       uint8_t CSCalc(UART_BUFF f)
Called By:   ��
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void MeterVol ( void ) //��ȡ�����й��ܵ��� 68 AA AA AA AA AA AA 68 11 04 33 34 34 35 B1 16
{

    uint8_t CS;
    RS485Tx2.Buff[0] = 0xFE; //ǰ���ֽ�
    RS485Tx2.Buff[1] = 0xFE;
    RS485Tx2.Buff[2] = 0xFE;
    RS485Tx2.Buff[3] = 0xFE;
    RS485Tx2.Buff[4] = 0x68; //֡��ʼ��

    RS485Tx2.Buff[5] = 0xaa;//MeterAddr[0];//��ַ��
    RS485Tx2.Buff[6] = 0xaa;//MeterAddr[1];
    RS485Tx2.Buff[7] = 0xaa;//MeterAddr[2];
    RS485Tx2.Buff[8] = 0xaa;//MeterAddr[3];
    RS485Tx2.Buff[9] = 0xaa;//MeterAddr[4];
    RS485Tx2.Buff[10] = 0xaa;//MeterAddr[5];

    RS485Tx2.Buff[11] = 0x68; //֡��ʼ��
    RS485Tx2.Buff[12] = 0x11; //������
    RS485Tx2.Buff[13] = 0x04; //�����򳤶�
    RS485Tx2.Buff[14] = 0x33; //���ݱ�ʶ��
    RS485Tx2.Buff[15] = 0x34;
    RS485Tx2.Buff[16] = 0x34;
    RS485Tx2.Buff[17] = 0x35;

    RS485Tx2.Len = 20;//���ͳ���
    CS = CSCalc ( RS485Tx2 );
    RS485Tx2.Buff[18] = CS; //У����
    RS485Tx2.Buff[19] = 0x16; //������

    RS485Ch2SendMsg();//��ȡ����ܵ�������֡

}

/*************************************************************
Function: void MeterCur(void)
Description: ������
Calls:       uint8_t CSCalc(UART_BUFF f)
Called By:   ��
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void MeterCur ( void ) //��ȡ�����й��ܵ��� 68 AA AA AA AA AA AA 68 11 04 33 34 35 35 B2 16
{

    uint8_t CS;
    RS485Tx2.Buff[0] = 0xFE; //ǰ���ֽ�
    RS485Tx2.Buff[1] = 0xFE;
    RS485Tx2.Buff[2] = 0xFE;
    RS485Tx2.Buff[3] = 0xFE;
    RS485Tx2.Buff[4] = 0x68; //֡��ʼ��

    RS485Tx2.Buff[5] = 0xaa;//MeterAddr[0];//��ַ��
    RS485Tx2.Buff[6] = 0xaa;//MeterAddr[1];
    RS485Tx2.Buff[7] = 0xaa;//MeterAddr[2];
    RS485Tx2.Buff[8] = 0xaa;//MeterAddr[3];
    RS485Tx2.Buff[9] = 0xaa;//MeterAddr[4];
    RS485Tx2.Buff[10] = 0xaa;//MeterAddr[5];

    RS485Tx2.Buff[11] = 0x68; //֡��ʼ��
    RS485Tx2.Buff[12] = 0x11; //������
    RS485Tx2.Buff[13] = 0x04; //�����򳤶�
    RS485Tx2.Buff[14] = 0x33; //���ݱ�ʶ��
    RS485Tx2.Buff[15] = 0x34;
    RS485Tx2.Buff[16] = 0x35;
    RS485Tx2.Buff[17] = 0x35;

    RS485Tx2.Len = 20;//���ͳ���
    CS = CSCalc ( RS485Tx2 );
    RS485Tx2.Buff[18] = CS; //У����
    RS485Tx2.Buff[19] = 0x16; //������

    RS485Ch2SendMsg();//��ȡ����ܵ�������֡

}

/*************************************************************
Function:     void MeterJs(void)
Description:  �㲥Уʱ
Calls:       ��
Called By:   ��
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void MeterJs ( void ) //�㲥Уʱ
{


}

/*************************************************************
Function:     uint8_t GetMeterOnlineSta(void)
Description:  ��ȡ���ͨ��״̬
Calls:       ��
Called By:   ��
Input:       ��
Output:      ��
Return:      0-���� 1-����
Others:      ��
*************************************************************/
uint8_t GetMeterOnlineSta(void)
{
    return MeterCommErr;
}

/*************************************************************
Function:     void MeterDeal ( void )
Description:  ������ж��Ƿ����
Calls:       ��
Called By:   ��
Input:       ��
Output:      ��
Return:      ��
Others:      ��
*************************************************************/
void MeterDeal ( void )
{
    GetMeterMsg();

    if ( MeterOnlineCnt >= 10 )         //10��ͨ�ų�ʱ����Ϊ���ͨ�Ź���
    {
        MeterCommErr = 1 ;
        ERRORLEDOPEN();
    }
    else
    {
        MeterCommErr = 0 ;
        ERRORLEDCLOSE();
    }
    
    if (SysTickCnt > RdMeterTicks + 1000 )//ÿ1s��һ��
    {
        MeterOnlineCnt += 1;
        if (Meter_counter == 1)
        {
            RdMeterTicks = SysTickCnt ;
            MeterCalc();//��������
            Meter_counter += 1;
        }
        else if (Meter_counter == 2)
        {       
            RdMeterTicks = SysTickCnt ;
            MeterVol();//������ѹ
            Meter_counter += 1;
        }
        else if (Meter_counter == 3)
        {
            RdMeterTicks = SysTickCnt ;
            MeterCur();//��������
            Meter_counter += 1;
        }
        else
        {
            Meter_counter = 1;
        }
    }
}












