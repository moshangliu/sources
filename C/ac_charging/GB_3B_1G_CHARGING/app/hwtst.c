
#include "drv_inc.h"
#include "Bsp_inc.h"
#include "app_inc.h"


extern UART_BUFF RS232Rx, RS232Tx;
extern UART_BUFF RS485Rx1, RS485Tx1;
extern UART_BUFF RS485Rx2, RS485Tx2;
uint8_t HwTestFlag, RelayFlag;
uint16_t HwTestCnt, TestCanCnt, Test485Cnt;
uint8_t RelaySta;
uint8_t TestCan, CanStates;
uint8_t Test485, RS485States;
uint8_t TestOutCnt;
uint8_t GetHwTestSta ( void )
{
    return HwTestFlag;
}
void HwTestInit ( void )
{
    RelayFlag = 0x55 ;
}
void HwTestIO ( void )
{
    uint8_t i, tmp;
    static uint32_t HwTestTicks;
     
    if (SysTickCnt >HwTestTicks + 1000 )
    {
        HwTestTicks = SysTickCnt ;
        RelaySta = ~ RelaySta ;
        tmp = RelaySta;

        if ( tmp & 0x1 )
        {
            SetOutput ( LED2 );
        }
        else
        {
            ClrOutput ( LED2 );
        }

        for ( i = 0; i < OUTPUT_NUM_MAX; i++ )
        {
            ClrOutput ( i + RELAY1 );
        }
        if (TestOutCnt>=OUTPUT_NUM_MAX)
        {
            TestOutCnt = 0;
        }
        SetOutput ( TestOutCnt + RELAY1 );
        TestOutCnt++;
    }
}

void HwTest485 ( void )
{
    uint8_t tmp, i;
    tmp = Test485 % 4 ;

    switch ( tmp )
    {
        case 0:
        {
            RS485Tx1.Buff[0] = 0;
            RS485Tx1.Buff[1] = 1;
            RS485Tx1.Buff[2] = 2;
            RS485Tx1.Buff[3] = 3;
            RS485Tx1.Buff[4] = 4;
            RS485Tx1.Buff[5] = 5;
            RS485Tx1.Buff[6] = 6;
            RS485Tx1.Buff[7] = 7;
            RS485Tx1.Len = 8;
            RS485Ch1SendMsg();
            Test485 = 1;
            Test485Cnt = 0;
            break;
        }
        case 1:
        {
            Test485Cnt++;

            if ( GetRs485Ch2Sta() == 1 )
            {
                for ( i = 0; i < 8; i++ )
                {
                    if ( RS485Rx2.Buff[i] == RS485Tx1.Buff[i] )
                    {
                        ;
                    }
                    else
                    {
                        RS485States = 0x3 ;//����
                        Test485 = 2;
                        break;
                    }
                }

                Test485 = 2;
            }

            if ( Test485Cnt > 1000 )
            {
                RS485States = 0x3 ;//����
                Test485 = 2;
            }

            break;
        }
        case 2:
        {
            RS485Tx2.Buff[0] = 0;
            RS485Tx2.Buff[1] = 1;
            RS485Tx2.Buff[2] = 2;
            RS485Tx2.Buff[3] = 3;
            RS485Tx2.Buff[4] = 4;
            RS485Tx2.Buff[5] = 5;
            RS485Tx2.Buff[6] = 6;
            RS485Tx2.Buff[7] = 7;
            RS485Tx2.Len = 8;
            RS485Ch2SendMsg();
            Test485 = 3;
            Test485Cnt = 0;
            break;
        }
        case 3:
        {
            Test485Cnt++;

            if ( GetRs485Ch1Sta() == 1 )
            {
                for ( i = 0; i < 8; i++ )
                {
                    if ( RS485Rx1.Buff[i] == RS485Tx2.Buff[i] )
                    {
                        ;
                    }
                    else
                    {
                        RS485States = 0x3 ;//����
                        Test485 = 0;
                        break;
                    }
                }

                Test485 = 0;
            }

            if ( Test485Cnt > 1000 )
            {
                RS485States = 0x3 ;//����
                Test485 = 0;
            }

            break;
        }
    }
}

void HwTestCan ( void )
{
    uint8_t tmp, i;
    CAN_MSG_Type  TxMsg, RxMsg;
    tmp = TestCan % 4 ;

    switch ( tmp )
    {
        case 0:
        {
            TxMsg.format = EXT_ID_FORMAT;
            TxMsg.id = 0;
            TxMsg.len = 8;
            TxMsg.type = DATA_FRAME;
            TxMsg.dataA[0] = 1 ; //
            TxMsg.dataA[1] = 2 ; //
            TxMsg.dataA[2] = 3 ; //
            TxMsg.dataA[3] = 4 ; //
            TxMsg.dataB[0] = 5 ; // ����
            TxMsg.dataB[1] = 6 ; //
            TxMsg.dataB[2] = 7 ; //
            TxMsg.dataB[3] = 8 ; //
            CAN_send ( CAN_1, &TxMsg );
            TestCan = 1;
            TestCanCnt = 0;
            break;
        }
        case 1:
        {
            TestCanCnt++;

            if ( CAN_receive ( CAN_2, &RxMsg ) == 0 )
            {
                for ( i = 0; i < 4; i++ )
                {
                    if ( ( RxMsg.dataA[i] == i + 1 ) && ( RxMsg.dataB[i] == i + 5 ) )
                    {
                        CanStates = 0;
                    }
                    else
                    {
                        CanStates = 0x3 ;//����
                        TestCan = 2;
                        break;
                    }
                }

                TestCan = 2;
            }

            if ( TestCanCnt > 1000 )
            {
                CanStates = 0x3 ;//����
                TestCan = 2;
            }

            break;
        }
        case 2:
        {
            TxMsg.format = EXT_ID_FORMAT;
            TxMsg.id = 0;
            TxMsg.len = 8;
            TxMsg.type = DATA_FRAME;
            TxMsg.dataA[0] = 1 ; //
            TxMsg.dataA[1] = 2 ; //
            TxMsg.dataA[2] = 3 ; //
            TxMsg.dataA[3] = 4 ; //
            TxMsg.dataB[0] = 5 ; // ����
            TxMsg.dataB[1] = 6 ; //
            TxMsg.dataB[2] = 7 ; //
            TxMsg.dataB[3] = 8 ; //
            CAN_send ( CAN_2, &TxMsg );
            TestCan = 3;
            TestCanCnt = 0;
            break;
        }
        case 3:
        {
            TestCanCnt++;

            if ( CAN_receive ( CAN_1, &RxMsg ) == 0 )
            {
                for ( i = 0; i < 4; i++ )
                {
                    if ( ( RxMsg.dataA[i] == i + 1 ) && ( RxMsg.dataB[i] == i + 5 ) )
                    {
                        CanStates = 0;
                    }
                    else
                    {
                        CanStates = 0x3 ;//����
                        TestCan = 0;
                        break;
                    }
                }

                TestCan = 0;
                break;
            }

            if ( TestCanCnt > 1000 )
            {
                CanStates = 0x3 ;//����
                TestCan = 0;
            }

            break;
        }
    }

}

void HwTestAdc ( void )
{

}
uint8_t crcl, crch;
uint16_t result = 0;
void HwModbus ( void )
{
    uint16_t addr, temp ;
    uint8_t i, len;

    if ( GetRs232Sta() == 1 ) //֡���ս������
    {
        if ((RS232Rx.Buff[0]==0x02)&&(RS232Rx.Buff[1]==0x00)) //������������Ϣ
        {
            return;
        }
        len = RS232Rx.Len ;
        RS232Rx.Flag = 0 ;
        RS232Rx.Len = 0;
        RS232Rx.Idx = 0 ;
        //if (RS232Rx.Buff[0] == SLAVE_ID)//���ж��Ƿ񱾻���ַ
        {
            CRC_Init ( CRC_POLY_CRC16 );
            //result ��8λ��ӦMODBUS CRC L ����8λ��ӦMODBUS CRC H
            result = CRC_CalcBlockChecksum ( RS232Rx.Buff, len - 2, CRC_WR_16BIT ); //����CRC
            crcl = result ;
            crch = result >> 8 ;

            if ( ( crcl == RS232Rx.Buff[len-2] ) && ( crch == RS232Rx.Buff[len-1] ) ) //�Ƚ�CRC
            {
                ;
            }
            else
            {
                return;
            }
        }

        switch ( RS232Rx.Buff[1] ) //������
        {
            case 0x03:// ����վ���յ��ģ���ַ 03 ��ַ�� ��ַ�� �Ĵ��������� �Ĵ��������� CRCL CRCH
            {
                // ���͸���վ�ģ���ַ 03 �ֽڳ��� ����1 ����2 ����N CRCL CRCH
                
                if (HwTestFlag==0)
                {
                    HwTestFlag = 1 ;
                    CAN_Init ( CAN_2, 250000 );
                    CAN_Init ( CAN_1, 250000 );
                    UartInit ( 0, 115200, UART_PARITY_NONE );
                    UartInit ( 1, 38400, UART_PARITY_NONE );
                    UartInit ( 2, 38400, UART_PARITY_EVEN );                    
                }
                RS232Tx.Buff[0] = RS232Rx.Buff[0] ;//��ַ
                RS232Tx.Buff[1] = RS232Rx.Buff[1] ;//03������

                temp = RS232Rx.Buff[4] ;
                temp <<= 8 ;
                temp |= RS232Rx.Buff[5] ;//temp��Ϊ��վҪ��ĳ���
                temp *= 2 ;              //1���Ĵ���Ϊ2�ֽڣ����Գ���X2
                RS232Tx.Buff[2] = temp ;//����
                len = temp ;
                i = 3 ;//����������ʼ��ַ
                temp = * ( ( uint16_t * ) ( &RS232Rx.Buff[2] ) );
                addr =  SWAP ( temp ) ;
                temp = len;

                while ( len )
                {
                    switch ( addr )
                    {
                        case 250:
                        {
                            //���ֽڣ�BIT1-485 2 BIT0-485 1
                            RS232Tx.Buff[i] = RS485States;
                            i++;
                            //���ֽڣ�BIT1-CAN 2 BIT0-CAN 1
                            RS232Tx.Buff[i] = CanStates;
                            i++;
                            addr++;
                            break;
                        }
                        case 251:
                        {
                            //���ֽڣ�BIT1-�ź�2 BIT0-�ź�1
                            RS232Tx.Buff[i] = 0 ;

                            if ( GetInputStatus ( INPUT1 ) )
                            {
                                RS232Tx.Buff[i] |= 0x1 ;
                            }

                            if ( GetInputStatus ( INPUT2 ) )
                            {
                                RS232Tx.Buff[i] |= 0x2 ;
                            }

                            i++;
                            //���ֽڣ�BIT5-BIT0 ���6-���1 RelaySta
                            RS232Tx.Buff[i] = RelaySta ;
                            i++;
                            addr++;
                            break;
                        }
                        case 252:
                        {
                            //�¶�1
                            // dealtemp ( 1 );
                            g_sAnalogValue.linkv = GetAnalogSta();
                            RS232Tx.Buff[i++] = g_sAnalogValue.linkv;
                            addr++;
                            break;
                        }
                        case 253:
                        {
                            //�¶�2
                            dealtemp ( 2 );
                            addr++;
                            break;
                        }
                        case 254:
                        {
                            //����
                            dealcurrent();
                            addr++;
                            break;
                        }
                        case 255:
                        {
                            //��˿
                            dealfuse();
                            addr++;
                            break;
                        }
                    }

                    len -= 2 ;
                }

                temp += 3 ;//���������� + ֡ͷ
                CRC_Init ( CRC_POLY_CRC16 );
                result = CRC_CalcBlockChecksum ( RS232Tx.Buff, temp, CRC_WR_16BIT );
                crcl = result ;
                crch = result >> 8 ;
                RS232Tx.Buff[temp] = crcl;
                RS232Tx.Buff[temp+1] = crch;

                RS232Tx.Len = temp + 2 ; //�����ͳ���
                RS232SendMsg();
                break;
            }
            case 0x10:// ����վ���յ��ģ���ַ 0x10 ��ַ�� ��ַ�� �Ĵ��������� �Ĵ��������� �ֽ��� ����1  ����2 ����N CRCL CRCH
            {
                // ���͸���վ�ģ���ַ 0x10 ��ַ�� ��ַ�� �Ĵ��������� �Ĵ��������� CRCL CRCH
                RS232Tx.Buff[0] = RS232Rx.Buff[0] ;//��ַ
                RS232Tx.Buff[1] = RS232Rx.Buff[1] ;//03������
                RS232Tx.Buff[2] = RS232Rx.Buff[2] ;//
                RS232Tx.Buff[3] = RS232Rx.Buff[3] ;//
                RS232Tx.Buff[4] = RS232Rx.Buff[4] ;//
                RS232Tx.Buff[5] = RS232Rx.Buff[5] ;//
                CRC_Init ( CRC_POLY_CRC16 );
                result = CRC_CalcBlockChecksum ( RS232Tx.Buff, 6, CRC_WR_16BIT );
                crcl = result ;
                crch = result >> 8 ;
                RS232Tx.Buff[6] = crcl;
                RS232Tx.Buff[7] = crch;

                RS232Tx.Len = 8 ;//�����ͳ���
                RS232SendMsg(); //�ȷ��ͻ�Ӧ����վ���������ݴ���ʱ���������ͨ�Ŵ���
                /*�������Ӿ��幦��ʵ��*/
                //��ȡ��ַ����ַ�� ��ַ��
                //��ȡ���ȣ��Ĵ��������� �Ĵ���������
                //ѭ���Ĵ�������
                //���ν����ݷŵ���Ӧ������
                break;
            }
            default:
                break;


        }

    }
}

void HwTestCntInc ( void )
{
    HwTestCnt++;

    if ( HwTestCnt > 1000 )
    {
        HwTestCnt = 0 ;
    }
}

void HwMain ( void )
{
    HwModbus();

    if ( GetHwTestSta() )
    {
        HwTestIO();
        HwTest485();
        HwTestCan();
        HwTestAdc();
    }
}



