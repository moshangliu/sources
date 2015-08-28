#ifndef __UART_H__
#define __UART_H__


/* Receive buffer size */
#define UART_RING_BUFSIZE	256

typedef struct
{
    uint8_t  Buff[UART_RING_BUFSIZE];   //���ջ�����
    uint8_t  Idx;                       //������ָ��
    uint8_t  Len;                       //���յ������ݳ��� �� ׼�����͵����ݳ���
    uint8_t  Flag;                      //������һ֡���ݱ�ǣ��� ������һ֡���ݱ��
} UART_BUFF;

extern UART_BUFF RS232Rx, RS232Tx;
extern UART_BUFF RS485Rx1, RS485Tx1;
extern UART_BUFF RS485Rx2, RS485Tx2;
extern void FrameIsEnd ( void );
void RS232SendMsg ( void );    //�������ݣ�����RS232Tx.Buff�г���RS232Tx.Len����
void RS485Ch1SendMsg ( void ); //�������ݣ�����RS485Tx1.Buff�г���RS485Tx1.Len����
void RS485Ch2SendMsg ( void ); // �������ݣ�����RS485Tx2.Buff�г���RS485Tx2.Len����
uint8_t GetRs232Sta ( void ); //��ȡRS232״̬��0--���� 2--æ  1--���յ�����֡ RS232Rx.Buff RS232Rx.Len
uint8_t GetRs485Ch1Sta ( void ); //��ȡRS485״̬��0--���� 2--æ  1--���յ�����֡ RS485Rx1.Buff RS485Rx1.Len
uint8_t GetRs485Ch2Sta ( void ); //��ȡRS485״̬��0--���� 2--æ  1--���յ�����֡ RS485Rx2.Buff RS485Rx2.Len
void UartInit ( uint8_t num, uint32_t baudrate, uint8_t parity ); //    ��ʼ��UART�ӿ�
void UartsInit ( void );


#endif