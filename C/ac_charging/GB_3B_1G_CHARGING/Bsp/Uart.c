
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_gpdma.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_crc.h"

#include "Uart.h"

#define FRAMESPACETIME 20

UART_BUFF RS232Rx, RS232Tx;
UART_BUFF RS485Rx1, RS485Tx1;
UART_BUFF RS485Rx2, RS485Tx2;

uint8_t FrameCntRS232;
uint8_t FrameCntRS4851;
uint8_t FrameCntRS4852;

#define RS232_UART LPC_UART3
#define RS232_TX_PIN GPDMA_CONN_UART3_Tx
#define RS232_IRQN UART3_IRQn
/************************** PRIVATE VARIABLES *************************/

// UART������
__IO uint32_t RS232_Err;

// UART������
__IO uint32_t RS4851_Err;

// UART������
__IO uint32_t RS4852_Err;

GPDMA_Channel_CFG_Type GPDMACfgTx;
/************************** PRIVATE FUNCTIONS *************************/
void DMA_IRQHandler ( void );

void UART_IntErr ( uint8_t bLSErrType );

uint32_t UARTReceive ( LPC_UART_TypeDef *UARTPort, uint8_t *rxbuf, uint32_t buflen );

/*************************************************************
  Function: void DMA_IRQHandler (void)
  Description: DMA�жϺ�����UART������ɴ���
  Calls:       void DMA_IRQHandler (void)
  Called By:   ��
  Input:       ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void DMA_IRQHandler ( void )
{
    uint32_t tmp;

    // Scan interrupt pending
    for ( tmp = 0; tmp <= 7; tmp++ )
    {
        if ( GPDMA_IntGetStatus ( GPDMA_STAT_INT, tmp ) )
        {
            // Check counter terminal status
            if ( GPDMA_IntGetStatus ( GPDMA_STAT_INTTC, tmp ) )
            {
                // Clear terminate counter Interrupt pending
                GPDMA_ClearIntPending ( GPDMA_STATCLR_INTTC, tmp );

                switch ( tmp )
                {
                    case 0:
                        RS232Tx.Flag = 0;
                        GPDMA_ChannelCmd ( 0, DISABLE );
                        break;
                    case 1:
                        RS485Tx1.Flag = 0;
                        GPDMA_ChannelCmd ( 1, DISABLE );
                        break;
                    case 2:
                        RS485Tx2.Flag = 0;
                        GPDMA_ChannelCmd ( 2, DISABLE );
                        break;
                    default:
                        break;
                }

            }

            // Check error terminal status
            if ( GPDMA_IntGetStatus ( GPDMA_STAT_INTERR, tmp ) )
            {
                // Clear error counter Interrupt pending
                GPDMA_ClearIntPending ( GPDMA_STATCLR_INTERR, tmp );

                switch ( tmp )
                {
                    case 0:
                        RS232_Err++;
                        GPDMA_ChannelCmd ( 0, DISABLE );
                        break;
                    case 1:
                        RS4851_Err++;
                        GPDMA_ChannelCmd ( 1, DISABLE );
                        break;
                    case 2:
                        RS4852_Err++;
                        GPDMA_ChannelCmd ( 1, DISABLE );
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
/*************************************************************
  Function: void UART0_IRQHandler (void)
  Description: UART0�жϺ�����RS232�����ж�
  Calls:
  Called By:   ��
  Input:       ��
  Output:      RS232Rx.Len RS232Rx.Idx RS232Rx.Flag
  Return:      ��
  Others:      ��
*************************************************************/
void UART0_IRQHandler ( void )
{
    uint32_t intsrc, tmp, tmp1;
    /* Determine the interrupt source */
    intsrc = UART_GetIntId ( RS232_UART );
    tmp = intsrc & UART_IIR_INTID_MASK;

    // Receive Line Status
    if ( tmp == UART_IIR_INTID_RLS )
    {
        // Check line status
        tmp1 = UART_GetLineStatus ( RS232_UART );
        // Mask out the Receive Ready and Transmit Holding empty status
        tmp1 &= ( UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
                  | UART_LSR_BI | UART_LSR_RXFE );

        // If any error exist
        if ( tmp1 )
        {
            RS232_Err++;//UART_DeInit(RS232_UART);UartInit ( 0, 115200, UART_PARITY_NONE );
        }
    }

    // Receive Data Available
    if ( ( tmp == UART_IIR_INTID_RDA ) )
    {
        RS232Rx.Len += UARTReceive ( RS232_UART, &RS232Rx.Buff[RS232Rx.Idx], 0 );
        RS232Rx.Idx = RS232Rx.Len ;
        FrameCntRS232 = 0 ;
    }

    //Character time-out
    if  ( tmp == UART_IIR_INTID_CTI )
    {
        RS232Rx.Len += UARTReceive ( RS232_UART, &RS232Rx.Buff[RS232Rx.Idx], 0 );
        RS232Rx.Idx = RS232Rx.Len ;
        //RS232Rx.Flag = 1 ;
        FrameCntRS232 = 0 ;
    }
}


/*************************************************************
  Function: void UART3_IRQHandler (void)
  Description: UART3�жϺ�����RS232�����ж�
  Calls:
  Called By:   ��
  Input:       ��
  Output:      RS232Rx.Len RS232Rx.Idx RS232Rx.Flag
  Return:      ��
  Others:      ��
*************************************************************/
void UART3_IRQHandler ( void )
{
    uint32_t intsrc, tmp, tmp1;

    /* Determine the interrupt source */
    intsrc = UART_GetIntId ( RS232_UART );
    tmp = intsrc & UART_IIR_INTID_MASK;

    // Receive Line Status
    if ( tmp == UART_IIR_INTID_RLS )
    {
        // Check line status
        tmp1 = UART_GetLineStatus ( RS232_UART );
        // Mask out the Receive Ready and Transmit Holding empty status
        tmp1 &= ( UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
                  | UART_LSR_BI | UART_LSR_RXFE );

        // If any error exist
        if ( tmp1 )
        {
            RS232_Err++;//UART_DeInit(RS232_UART);UartInit ( 0, 115200, UART_PARITY_NONE );
        }
    }

    // Receive Data Available
    if ( ( tmp == UART_IIR_INTID_RDA ) )
    {
        RS232Rx.Len += UARTReceive ( RS232_UART, &RS232Rx.Buff[RS232Rx.Idx], 0 );
        RS232Rx.Idx = RS232Rx.Len ;
        FrameCntRS232 = 0 ;
    }

    //Character time-out
    if  ( tmp == UART_IIR_INTID_CTI )
    {
        RS232Rx.Len += UARTReceive ( RS232_UART, &RS232Rx.Buff[RS232Rx.Idx], 0 );
        RS232Rx.Idx = RS232Rx.Len ;
        RS232Rx.Flag = 1 ;
    }
}

/*************************************************************
  Function: void UART1_IRQHandler (void)
  Description: UART1�жϺ�����RS485�����ж�
  Calls:
  Called By:   ��
  Input:       ��
  Output:      RS485Rx1.Len RS485Rx1.Idx RS485Rx1.Flag
  Return:      ��
  Others:      ��
*************************************************************/
void UART1_IRQHandler ( void )
{
    uint32_t intsrc, tmp, tmp1;

    /* Determine the interrupt source */
    intsrc = UART_GetIntId ( ( LPC_UART_TypeDef * ) LPC_UART1 );
    tmp = intsrc & UART_IIR_INTID_MASK;

    // Receive Line Status
    if ( tmp == UART_IIR_INTID_RLS )
    {
        // Check line status
        tmp1 = UART_GetLineStatus ( ( LPC_UART_TypeDef * ) LPC_UART1 );
        // Mask out the Receive Ready and Transmit Holding empty status
        tmp1 &= ( UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
                  | UART_LSR_BI | UART_LSR_RXFE );

        // If any error exist
        if ( tmp1 )
        {
            RS4851_Err++;
        }
    }

    // Receive Data Available
    if ( ( tmp == UART_IIR_INTID_RDA ) )
    {
        RS485Rx1.Len += UARTReceive ( ( LPC_UART_TypeDef * ) LPC_UART1, &RS485Rx1.Buff[RS485Rx1.Idx], 0 );
        RS485Rx1.Idx = RS485Rx1.Len ;
        FrameCntRS4851 = 0 ;
    }

    //Character time-out
    if  ( tmp == UART_IIR_INTID_CTI )
    {
        RS485Rx1.Len += UARTReceive ( ( LPC_UART_TypeDef * ) LPC_UART1, &RS485Rx1.Buff[RS485Rx1.Idx], 0 );
        RS485Rx1.Idx = RS485Rx1.Len ;
        RS485Rx1.Flag = 1 ;
    }
}

/*************************************************************
  Function: void UART2_IRQHandler (void)
  Description: UART2�жϺ�����RS485�����ж�
  Calls:
  Called By:   ��
  Input:       ��
  Output:      RS485Rx1.Len RS485Rx1.Idx RS485Rx1.Flag
  Return:      ��
  Others:      ��
*************************************************************/
void UART2_IRQHandler ( void )
{
    uint32_t intsrc, tmp, tmp1;

    /* Determine the interrupt source */
    intsrc = UART_GetIntId ( LPC_UART2 );
    tmp = intsrc & UART_IIR_INTID_MASK;

    // Receive Line Status
    if ( tmp == UART_IIR_INTID_RLS )
    {
        // Check line status
        tmp1 = UART_GetLineStatus ( LPC_UART2 );
        // Mask out the Receive Ready and Transmit Holding empty status
        tmp1 &= ( UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
                  | UART_LSR_BI | UART_LSR_RXFE );

        // If any error exist
        if ( tmp1 )
        {
            RS4852_Err++;
        }
    }

    // Receive Data Available
    if ( ( tmp == UART_IIR_INTID_RDA ) )
    {
        RS485Rx2.Len += UARTReceive ( LPC_UART2, &RS485Rx2.Buff[RS485Rx2.Idx], 0 );
        RS485Rx2.Idx = RS485Rx2.Len ;
        FrameCntRS4852 = 0 ;
    }

    //Character time-out
    if  ( tmp == UART_IIR_INTID_CTI )
    {
        RS485Rx2.Len += UARTReceive ( LPC_UART2, &RS485Rx2.Buff[RS485Rx2.Idx], 0 );
        RS485Rx2.Idx = RS485Rx2.Len ;
        RS485Rx2.Flag = 1 ;

    }
}

/*************************************************************
  Function: uint32_t UARTReceive(LPC_UART_TypeDef *UARTPort, uint8_t *rxbuf, uint32_t buflen)
  Description: UART���ݽ��պ�������ȡUART FIFO������
  Calls:       UART_Receive
  Called By:   UARTx_IRQHandler
  Input:       UARTPort ָ��UART�˿�
               buflen ��δʹ��
  Output:      rxbuf ָ�����ջ�����
  Return:      ���յ������ݳ���
  Others:      ��
*************************************************************/
uint32_t UARTReceive ( LPC_UART_TypeDef *UARTPort, uint8_t *rxbuf, uint32_t buflen )
{
    uint8_t tmpc;
    uint32_t rLen, bytes;
    bytes = 0 ;

    while ( 1 )
    {
        // Call UART read function in UART driver
        rLen = UART_Receive ( UARTPort, &tmpc, 1, NONE_BLOCKING );

        // If data received
        if ( rLen )
        {
            *rxbuf = tmpc;
            rxbuf++;
            bytes++;
        }
        // no more data
        else
        {
            break;
        }
    }

    return bytes;
}

/*************************************************************
  Function: void RS232SendMsg(void)
  Description: RS232�������ݺ�����������������
  Calls:       GPDMA_Setup
               GPDMA_ChannelCmd
  Called By:
  Input:       RS232Tx.Buff
               RS232Tx.Len
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void RS232SendMsg ( void )
{
    GPDMACfgTx.ChannelNum = 0;
    // Source memory
    GPDMACfgTx.SrcMemAddr = ( uint32_t ) &RS232Tx.Buff;
    // Transfer size
    GPDMACfgTx.TransferSize = RS232Tx.Len;
    // Destination connection
    GPDMACfgTx.DstConn = RS232_TX_PIN;
    GPDMA_Setup ( &GPDMACfgTx );
    // Enable GPDMA channel 0
    GPDMA_ChannelCmd ( 0, ENABLE );
}

/*************************************************************
  Function: void RS485Ch1SendMsg(void)
  Description: RS485�������ݺ�����������������
  Calls:       GPDMA_Setup
               GPDMA_ChannelCmd
  Called By:
  Input:       RS485Tx1.Buff
               RS485Tx1.Len
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void RS485Ch1SendMsg ( void )
{
    GPDMACfgTx.ChannelNum = 1;
    // Source memory
    GPDMACfgTx.SrcMemAddr = ( uint32_t ) &RS485Tx1.Buff;
    // Transfer size
    GPDMACfgTx.TransferSize = RS485Tx1.Len;
    // Destination connection
    GPDMACfgTx.DstConn = GPDMA_CONN_UART1_Tx;
    GPDMA_Setup ( &GPDMACfgTx );
    // Enable GPDMA channel 0
    GPDMA_ChannelCmd ( 1, ENABLE );
    RS485Tx1.Flag = 1;
}

/*************************************************************
  Function: void RS485Ch2SendMsg(void)
  Description: RS485�������ݺ�����������������
  Calls:       GPDMA_Setup
               GPDMA_ChannelCmd
  Called By:
  Input:       RS485Tx2.Buff
               RS485Tx2.Len
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void RS485Ch2SendMsg ( void )
{
    GPDMACfgTx.ChannelNum = 2;
    // Source memory
    GPDMACfgTx.SrcMemAddr = ( uint32_t ) &RS485Tx2.Buff;
    // Transfer size
    GPDMACfgTx.TransferSize = RS485Tx2.Len;
    // Destination connection
    GPDMACfgTx.DstConn = GPDMA_CONN_UART2_Tx;
    GPDMA_Setup ( &GPDMACfgTx );
    // Enable GPDMA channel 0
    GPDMA_ChannelCmd ( 2, ENABLE );
    RS485Tx2.Flag = 1;
}

/*************************************************************
Function: void UartInit(uint8_t num,uint32_t baudrate,uint8_t parity)
  Description: ���ڳ�ʼ���������ڳ�ʼ��RS232��RS485 ��UART����Ϊ�жϽ��գ�DMA���ͣ�RS485�Զ��л�����
  Calls:
  Called By:   main()
  Input:       num ���ں�0��1��2
               baudrate ������
               parity У�鷽ʽ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void UartInit ( uint8_t num, uint32_t baudrate, uint8_t parity )
{
    //	uint32_t idx;
    // RS485 configuration
    UART1_RS485_CTRLCFG_Type rs485cfg;
    // UART Configuration structure variable
    UART_CFG_Type UARTConfigStruct;
    // UART FIFO configuration Struct variable
    UART_FIFO_CFG_Type UARTFIFOConfigStruct;
    GPDMA_Channel_CFG_Type GPDMACfg;

    UART_ConfigStructInit ( &UARTConfigStruct );

    UARTConfigStruct.Baud_rate = baudrate;
    UARTConfigStruct.Parity = parity;

    UART_FIFOConfigStructInit ( &UARTFIFOConfigStruct );

    // Enable DMA mode in UART
    UARTFIFOConfigStruct.FIFO_DMAMode = ENABLE;
    // Destination memory - don't care
    GPDMACfgTx.DstMemAddr = 0;
    // Transfer width - don't care
    GPDMACfgTx.TransferWidth = 0;
    // Transfer type
    GPDMACfgTx.TransferType = GPDMA_TRANSFERTYPE_M2P;
    // Source connection - don't care
    GPDMACfgTx.SrcConn = 0;
    // Linker List Item - unused
    GPDMACfgTx.DMALLI = 0;

    rs485cfg.AutoDirCtrl_State = ENABLE;
    rs485cfg.DirCtrlPol_Level = SET;
    rs485cfg.DelayValue = 50;
    rs485cfg.NormalMultiDropMode_State = ENABLE;
    rs485cfg.AutoAddrDetect_State = DISABLE;

    rs485cfg.Rx_State = ENABLE;

    if ( num == 0 )
    {
        //PINSEL_ConfigPin(0,2,1); //UART0
        //PINSEL_ConfigPin(0,3,1); //UART0

        PINSEL_ConfigPin ( 0, 25, 0x03 );
        PINSEL_ConfigPin ( 0, 26, 0xb3 );
        // Initalize UART0 peripheral with given to corresponding parameter
        UART_Init ( RS232_UART, &UARTConfigStruct );
        // Initialize FIFO for UART0 peripheral
        UART_FIFOConfig ( RS232_UART, &UARTFIFOConfigStruct );

        // Enable UART Transmit
        UART_TxCmd ( RS232_UART, ENABLE );
        // channel 0
        GPDMACfgTx.ChannelNum = 0;
        // Source memory
        GPDMACfgTx.SrcMemAddr = ( uint32_t ) &RS232Tx.Buff;
        // Transfer size
        GPDMACfgTx.TransferSize = sizeof ( RS232Tx.Buff );
        // Destination connection
        GPDMACfgTx.DstConn = RS232_TX_PIN;

        RS232Tx.Flag = 0;

        RS232_Err = 0;

        UART_IntConfig ( RS232_UART, UART_INTCFG_RBR, ENABLE );

        UART_IntConfig ( RS232_UART, UART_INTCFG_RLS, ENABLE );

        NVIC_SetPriority ( RS232_IRQN, ( ( 0x01 << 3 ) | 0x01 ) );

        NVIC_EnableIRQ ( RS232_IRQN );
        RS232Rx.Flag = 0 ;
        RS232Rx.Len = 0 ;
        RS232Rx.Idx = 0 ;
    }

    if ( num == 1 )
    {
        PINSEL_ConfigPin ( 2, 0, 2 );
        PINSEL_ConfigPin ( 2, 1, 2 );
        PINSEL_ConfigPin ( 2, 5, 2 ); //U1_DTR
        rs485cfg.DirCtrlPin = UART1_RS485_DIRCTRL_DTR;
        UART_Init ( ( LPC_UART_TypeDef * ) LPC_UART1, &UARTConfigStruct );
        UART_FIFOConfig ( ( LPC_UART_TypeDef * ) LPC_UART1, &UARTFIFOConfigStruct );
        UART_RS485Config ( ( LPC_UART_TypeDef * ) LPC_UART1, &rs485cfg );
        // Enable UART Transmit
        UART_TxCmd ( ( LPC_UART_TypeDef * ) LPC_UART1, ENABLE );
        GPDMACfgTx.ChannelNum = 1;
        // Source memory
        GPDMACfgTx.SrcMemAddr = ( uint32_t ) &RS485Tx1.Buff;
        // Transfer size
        GPDMACfgTx.TransferSize = sizeof ( RS485Tx1.Buff );
        // Destination connection
        GPDMACfgTx.DstConn = GPDMA_CONN_UART1_Tx;
        /* Reset terminal counter */
        RS485Tx1.Flag = 0;
        /* Reset Error counter */
        RS4851_Err = 0;
        /* Enable UART Rx interrupt */
        UART_IntConfig ( ( LPC_UART_TypeDef * ) LPC_UART1, UART_INTCFG_RBR, ENABLE );
        /* Enable UART line status interrupt */
        UART_IntConfig ( ( LPC_UART_TypeDef * ) LPC_UART1, UART_INTCFG_RLS, ENABLE );
        /* preemption = 1, sub-priority = 1 */
        NVIC_SetPriority ( UART1_IRQn, ( ( 0x01 << 3 ) | 0x01 ) );

        /* Enable Interrupt for UART0 channel */
        NVIC_EnableIRQ ( UART1_IRQn );
    }

    if ( num == 2 )
    {
        PINSEL_ConfigPin ( 2, 8, 2 );
        PINSEL_ConfigPin ( 2, 9, 2 );
        PINSEL_ConfigPin ( 2, 6, 4 ); //U2_OE

        UART_Init ( LPC_UART2, &UARTConfigStruct );
        UART_FIFOConfig ( LPC_UART2, &UARTFIFOConfigStruct );
        UART_RS485Config ( ( LPC_UART_TypeDef * ) LPC_UART2, &rs485cfg );
        // Enable UART Transmit
        UART_TxCmd ( LPC_UART2, ENABLE );
        GPDMACfgTx.ChannelNum = 2;
        // Source memory
        GPDMACfgTx.SrcMemAddr = ( uint32_t ) &RS485Tx2.Buff;
        // Transfer size
        GPDMACfgTx.TransferSize = sizeof ( RS485Tx2.Buff );
        // Destination connection
        GPDMACfgTx.DstConn = GPDMA_CONN_UART2_Tx;
        /* Reset terminal counter */
        RS485Tx2.Flag = 0;
        /* Reset Error counter */
        RS4852_Err = 0;
        /* Enable UART Rx interrupt */
        UART_IntConfig ( LPC_UART2, UART_INTCFG_RBR, ENABLE );
        /* Enable UART line status interrupt */
        UART_IntConfig ( LPC_UART2, UART_INTCFG_RLS, ENABLE );
        /* preemption = 1, sub-priority = 1 */
        NVIC_SetPriority ( UART2_IRQn, ( ( 0x01 << 3 ) | 0x01 ) );

        /* Enable Interrupt for UART0 channel */
        NVIC_EnableIRQ ( UART2_IRQn );
    }

    /* Initialize GPDMA controller */
    GPDMA_Init();

    /* Setting GPDMA interrupt */
    // Disable interrupt for DMA
    NVIC_DisableIRQ ( DMA_IRQn );
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority ( DMA_IRQn, ( ( 0x01 << 3 ) | 0x01 ) );

    // Setup channel with given parameter
    GPDMA_Setup ( &GPDMACfgTx );

    // Enable interrupt for DMA
    NVIC_EnableIRQ ( DMA_IRQn );

    // Enable GPDMA channel 0
    //GPDMA_ChannelCmd(0, ENABLE);
    CRC_Init ( CRC_POLY_CRC16 );

}

/*************************************************************
Function: uint8_t GetRs232Sta(void)
  Description: ��ȡRS232״̬��0--���� 2--æ  1--���յ�����֡ RS232Rx.Buff RS232Rx.Len
  Calls:
  Called By:
  Input:
  Output:      ��
  Return:      RS232״̬
  Others:      ��
*************************************************************/
uint8_t GetRs232Sta ( void )
{
    uint8_t sta = 0 ;

    if ( RS232Tx.Flag == 1 )
    {
        sta = 2 ;
    }
    else
    {
        sta = RS232Rx.Flag ;
    }

    return sta ;
}


/*************************************************************
Function: uint8_t GetRs485Ch1Sta(void)
  Description: ��ȡRS485״̬��0--���� 2--æ  1--���յ�����֡ RS232Rx.Buff RS232Rx.Len
  Calls:
  Called By:
  Input:
  Output:      ��
  Return:      RS485״̬
  Others:      ��
*************************************************************/
uint8_t GetRs485Ch1Sta ( void )
{
    uint8_t sta = 0 ;

    if ( RS485Tx1.Flag == 1 )
    {
        sta = 2 ;
    }
    else
    {
        sta = RS485Rx1.Flag ;
    }

    return sta ;
}

/*************************************************************
Function: uint8_t GetRs485Ch2Sta(void)
  Description: ��ȡRS485״̬��0--���� 2--æ  1--���յ�����֡ RS232Rx.Buff RS232Rx.Len
  Calls:
  Called By:
  Input:
  Output:      ��
  Return:      RS485״̬
  Others:      ��
*************************************************************/
uint8_t GetRs485Ch2Sta ( void )
{
    uint8_t sta = 0 ;

    if ( RS485Tx2.Flag == 1 )
    {
        sta = 2 ;
    }
    else
    {
        sta = RS485Rx2.Flag ;
    }

    return sta ;
}

/*************************************************************
Function: void FrameIsEnd(void)
  Description: �жϴ��ڽ����Ƿ������Ŀǰ�ڽ���RDA�жϺ���20ms���ڲ��������жϣ���Ϊ֡����
  Calls:
  Called By:   SysTick_Handler
  Input:       ��
  Output:      ֡�������
  Return:      ��
  Others:      ��
*************************************************************/
void FrameIsEnd ( void )
{
    if ( RS232Rx.Len )
    {
        FrameCntRS232++;

        if ( FrameCntRS232 > FRAMESPACETIME )
        {
            RS232Rx.Flag = 1 ;
        }
    }

    if ( RS485Rx1.Len )
    {
        FrameCntRS4851++;

        if ( FrameCntRS4851 > FRAMESPACETIME )
        {
            RS485Rx1.Flag = 1 ;
        }
    }

    if ( RS485Rx2.Len )
    {
        FrameCntRS4852++;

        if ( FrameCntRS4852 > 100 )
        {
            RS485Rx2.Flag = 1 ;
        }
    }
}

void RS232_DEAL ( void )
{
    uint16_t result = 0 ;

    if ( RS232Rx.Flag == 1 )
    {
        // Check whether if there's any character received, then print it back
        if ( RS232Rx.Buff[0] == 0X02 )
        {
            //result ��8λ��ӦMODBUS CRC L ����8λ��ӦMODBUS CRC H
            result = CRC_CalcBlockChecksum ( RS232Rx.Buff, RS232Rx.Len, CRC_WR_16BIT );

            for ( uint8_t i = 0; i < 10; i++ )
            {
                RS232Tx.Buff[i] = i ;
            }

            RS232Tx.Len = 10 ;
            RS232Tx.Flag = 1 ;
            RS232SendMsg();
        }

        RS232Rx.Flag = 0 ;
        RS232Rx.Len = 0 ;
        RS232Rx.Idx = 0 ;
    }
}

void UartsInit ( void )
{
    UartInit ( 1, 9600, UART_PARITY_NONE );    //485--1
    UartInit ( 2, 2400, UART_PARITY_EVEN );     //485--2
    UartInit ( 0, 38400, UART_PARITY_NONE );     //232
}