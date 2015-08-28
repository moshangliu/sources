/**********************************************************
  ���յº�����Դ�Ƽ����޹�˾
  File name:  AT45DB081.c	              // �ļ���
  Author:     ����                    // ����
  Version:    v1.00                   // �汾��Ϣ
  Date:       2015-4-15               // �������
  Description:  	              //AT45DB081����������,ʵ�ּ�¼���¼��Ĵ洢
  Others:		              // ��
  Function List:	              // void SaveRecord(uint8_t *buff)
                                         void LoadRecord(uint32_t num,uint8_t *buff)
                                         void SaveEvent(uint8_t *buff)
                                         void LoadEvent(uint32_t num,uint8_t *buff)
                                         void SaveAlarm(uint8_t *buff)
                                         void LoadAlarm(uint32_t num,uint8_t *buff)
                                         void FlashInit(void)
                                         void ErasingDataFlash(void)
                                         uint16_t GetRecordNum(void);
                                         uint16_t GetEventNum(void);
                                         uint16_t GetAlarmNum(void);
  History: 2012-09-13 �������������Զ�ʶ��FLASH�ͺţ�����AT45DB2562֮ǰ�����ͺ�
**********************************************************/


#include "board.h"
/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize    0x100

/* Private define ------------------------------------------------------------*/
#define SE         0x7C  /* Sector Erase instruction */
#define PE         0x81  /* Page Erase instruction */

/* Private define ------------------------------------------------------------*/
#define WRITE      0x82  /* Write to Memory instruction */
#define READ       0xD3  /* Read from Memory instruction */
#define RDSR       0xD7  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define PE         0x81  /* Page Erase instruction */
#define BE1        0xC7  /* Bulk Erase instruction */
#define BE2        0x94  /* Bulk Erase instruction */
#define BE3        0x80  /* Bulk Erase instruction */
#define BE4        0x9A  /* Bulk Erase instruction */

#define BUSY_Flag  0x80 /* Ready/busy status flag */

#define Dummy_Byte 0xA5
#define BUFFER_1_READ 0xd1 //0xD4 
// buffer 2 read
#define BUFFER_2_READ 0xd3 //0xD6 

// buffer 1 write
#define BUFFER_1_WRITE 0x84

// buffer 2 write
#define BUFFER_2_WRITE 0x87

// buffer 1 to main memory page program with built-in erase
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 0x83

// buffer 2 to main memory page program with built-in erase
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86

// buffer 1 to main memory page program without built-in erase
#define B1_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x88

// buffer 2 to main memory page program without built-in erase
#define B2_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x89

// main memory page program through buffer 1
#define MM_PAGE_PROG_THROUGH_B1 0x82

// main memory page program through buffer 2
#define MM_PAGE_PROG_THROUGH_B2 0x85

// auto page rewrite through buffer 1
#define AUTO_PAGE_REWRITE_THROUGH_B1 0x58

// auto page rewrite through buffer 2
#define AUTO_PAGE_REWRITE_THROUGH_B2 0x59

// main memory page compare to buffer 1
#define MM_PAGE_TO_B1_COMP 0x60

// main memory page compare to buffer 2
#define MM_PAGE_TO_B2_COMP 0x61

// main memory page to buffer 1 transfer
#define MM_PAGE_TO_B1_XFER 0x53

// main memory page to buffer 2 transfer
#define MM_PAGE_TO_B2_XFER 0x55

// DataFlash status register for reading density, compare status,
// and ready/busy status
#define STATUS_REGISTER 0xD7

// main memory page read
#define MAIN_MEMORY_PAGE_READ 0x52

// erase a 528 byte page
#define PAGE_ERASE 0x81

// erase 512 pages
#define BLOCK_ERASE 0x50

#define DF_READ_BUFFER 1
#define DF_WRITE_BUFFER 0
#define DF_PAGE_SIZE 528
#define DF_READ_BUFFER 1
#define DF_WRITE_BUFFER 0

#define DF_BUFF_SIZE 264
#define NUMDATAFLASH 10


#define DF_RECORD_LEN 74
#define DF_RECORD_BASE 0
#define DF_RECORD_NUM_PER_PAGE 3 //3*74=222

#define DF_ALARM_LEN  8
#define DF_ALARM_BASE DF_RECORD_LEN*DF_RECORD_NUM_PER_PAGE
#define DF_ALARM_NUM_PER_PAGE 2  //2*8=16   222+16 =238

#define DF_EVENT_LEN  15
#define DF_EVENT_BASE DF_ALARM_BASE+DF_ALARM_LEN*DF_ALARM_NUM_PER_PAGE
#define DF_EVENT_NUM_PER_PAGE 1  //1*15=15   15+236 =251

#define RECORD_MAX 500

/*
 * ��ֲ�������ʱ��Ҫ�޸����µĺ������ 
 */
#define     SSEL_EN         (0 << 16)
#define     SSEL_DIS        (1 << 16)
#define     EOT_EN          (1 << 20)
#define     EOT_DIS         (0 << 20)
#define     EOF_EN          (1 << 21)
#define     EOF_DIS         (0 << 21)
#define     RXIGNORE_EN     (1 << 22)
#define     RXIGNORE_DIS    (0 << 22)
#define     FLEN(n)         (((n) - 1) << 24)
uint32_t RecordNum, RecordFlag; //��ǰ��¼�ţ���¼��500�����
uint32_t EventNum, EventFlag;
uint32_t AlarmNum, AlarmFlag;

typedef struct
{
    //AT45DB FLASH�ͺż����

    unsigned int pageNumber;
    unsigned int hasBinaryPage;
    unsigned int pageSize;
    unsigned int pageOffset;
    unsigned char id;
    const char *name;

} At45Desc;

static const At45Desc at45Devices[] =
{
    {  512,  1, 264,   9, 0x0C, "AT45DB011D"},
    { 1024,  1, 264,   9, 0x14, "AT45DB021D"},
    { 2048,  1, 264,   9, 0x1C, "AT45DB041D"},
    { 4096,  1, 264,   9, 0x24, "AT45DB081D"},
    { 4096,  1, 528,  10, 0x2C, "AT45DB161D"},
    { 8192,  1, 528,  10, 0x34, "AT45DB321D"},
    { 8192,  1, 1056, 11, 0x3C, "AT45DB642D"},
    {16384,  1, 1056, 11, 0x10, "AT45DB1282"},
    {16384,  1, 2112, 12, 0x18, "AT45DB2562"},
    {32768,  1, 2112, 12, 0x20, "AT45DB5122"}
};

uint8_t DfBuff[DF_BUFF_SIZE] ; //������

uint8_t BinaryPageFlag, FlashType; //ҳ��ʽ��FLASH�ͺ�
uint16_t PageSize; //ҳ��С

/************************************************************************
** �������ܣ�ͨ��Ӳ��SPI����һ���ֽڵ�M45PE161
** ��ڲ���: TxData  ������ַ
** ���ڲ���: ���յ�������
** ��    ע: ��
************************************************************************/
uint8_t SPIx_ReadWriteByte ( LPC_SPI_T *pSPI, uint8_t TxData )
{
    uint8_t temp;
    pSPI->TXDAT = TxData;							// ���͸��������Բ���ʱ��,���ݱ���û���ô�

    while ( 1 == ( pSPI->STAT&0x02 ) );			// �ȴ�SPIF��λ�����ȴ����ݷ������

    temp = pSPI->STAT;								// ���SPIF��־

    return ( uint8_t ) ( pSPI->RXDAT );					// ���ؽ��յ�������
}
void CE_Low ( void )
{
    ;//Chip_GPIO_SetPinState ( LPC_GPIO_PORT, 0 ,19, 0 ) ; //ƬѡΪ��
}

void CE_High ( void )
{
    ;//Chip_GPIO_SetPinState ( LPC_GPIO_PORT, 0 ,19, 1 ) ; //ƬѡΪ��
}
/*********************************************************************************************************
** ��������: SendRecv_Byte
** �������ܣ�һ�δ����ڲ��ĵ��ֽ�֡���������
** �������: �������Ϸ��ͳ�������
** �������: �������Ͻ��յ�������
** �� �� ֵ����
*********************************************************************************************************/
uint8_t SendRecv_Byte (uint8_t ucData)
{
    while (!(LPC_SPI0->STAT & (1 << 1)));                               /* �ȴ�����׼������             */
    LPC_SPI0->TXDATCTL = FLEN(8) | EOF_EN | ucData;                     /* 8 λ��֡����                 */    
    
    while (!(LPC_SPI0->STAT & (1 << 0)));                               /* �ȴ������������             */
    ucData = LPC_SPI0->RXDAT;                                           /* ��������                     */
    
    return ucData;
}

/*********************************************************************************************************
** ��������: SendRecv_Start
** �������ܣ�һ�δ�����ʼ�ĵ��ֽ�֡���������
** �������: �������Ϸ��ͳ�������
** �������: �������Ͻ��յ�������
** �� �� ֵ����
*********************************************************************************************************/
uint8_t SendRecv_Start (uint8_t ucData)
{
    while (!(LPC_SPI0->STAT & (1 << 1)));                               /* �ȴ�����׼������             */
    LPC_SPI0->TXDATCTL = FLEN(8) | EOF_EN | SSEL_EN | ucData;           /* 8 λ�����俪ʼ��֡����       */    
    
    while (!(LPC_SPI0->STAT & (1 << 0)));                               /* �ȴ������������             */
    ucData = LPC_SPI0->RXDAT;                                           /* ��������                     */
    
    return ucData;
}

/*********************************************************************************************************
** ��������: SendRecv_Stop
** �������ܣ�һ�δ�������ĵ��ֽ�֡���������
** �������: �������Ϸ��ͳ�������
** �������: �������Ͻ��յ�������
** �� �� ֵ����
*********************************************************************************************************/
uint8_t SendRecv_Stop (uint8_t ucData)
{
    while (!(LPC_SPI0->STAT & (1 << 1)));                               /* �ȴ�����׼������             */
    LPC_SPI0->TXDATCTL = FLEN(8) | EOT_EN | ucData;                     /* 8 λ���������               */    
    
    while (!(LPC_SPI0->STAT & (1 << 0)));                               /* �ȴ������������             */
    ucData = LPC_SPI0->RXDAT;                                           /* ��������                     */
    
    return ucData;
}
/*************************************************************
  Function: uint8_t GetFlashStatus()
  Description: ��Flash��ǰ״̬Bit7=1:���� 0:æ Bit6=1:�ȽϽ����ƥ�� 0:ƥ�� Bit5-Bit2:оƬ��־
  Calls:
  Called By:
  Input:       ��
  Output:      ��
  Return:      ״̬�Ĵ���
  Others:      ��
*************************************************************/
uint8_t GetFlashStatus() //
{
    uint8_t  ret,ret1;
    CE_Low();
    SendRecv_Start ( STATUS_REGISTER );
    ret1 = SendRecv_Byte ( Dummy_Byte );
    ret = SendRecv_Stop ( Dummy_Byte );
    CE_High();
    return ret1;
}

/*************************************************************
  Function: void FlashInit(void)
  Description: ��ʼ��SPI�ӿڣ���ʼ��������
  Calls:
  Called By:   main()
  Input:       ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void FlashInit ( void )
{
    uint32_t i;
    uint8_t sta;
    LPC_SYSCON->SYSAHBCLKCTRL |=  (1 << 11);                            /* ��ʼ��SPI0 AHBʱ��           */
    LPC_SYSCON->PRESETCTRL    &= ~(1 << 0);                             /* ��λSPI0                     */
    LPC_SYSCON->PRESETCTRL    |=  (1 << 0);
    
    LPC_SPI0->DIV = SystemCoreClock / 1000000 - 1;                      /* ����ģʽ��SCKƵ�ʷ�Ƶ����    */
    LPC_SPI0->DLY = (0 << 0) |                                          /* ��ʼ�����ӳ٣�0 * T_SCK      */
                    (0 << 4) |                                          /* ���������ӳ٣�0 * T_SCK      */
                    (0 << 8) |                                          /* ֡������ӳ٣�0 * T_SCK      */
                    (0 << 12);                                          /* ��������ӳ٣�0 * T_SCK      */
    
    /*
     * TXCTRL ���Ϳ��������ã�
     * TXCTRL
     *      16������ӻ�ѡ��    0��ѡ��ӻ�     1��ȡ��ѡ��ӻ�     ����д����ǰSSEL�źŵ���Ч��
     *      20�������������    0�����䲻����   1���������         ����ÿ��д���ݺ�SSEL�źŵ���Ч��
     *      21��֡��������      0��֡������     1��֡����           �����´�д����ǰ�Ƿ����֡���ӳ�
     *      22�����պ��Կ���    0�������Խ���   1�����Խ���
     *   27:24��֡����          0x0 ~ 0xF����Ӧ 1 ~ 16 λ֡����
     */
    LPC_SPI0->TXCTRL = (0 << 16) |                                      /* �ӻ�ѡ��                     */
                       (0 << 20) |                                      /* ���䲻����                   */
                       (1 << 21) |                                      /* ֡����                       */
                       (0 << 22) |                                      /* �����Խ���                   */
                       (7 << 24);                                       /* ֡���ȣ�8λ                  */
    
    /*
     * CFG ���üĴ������ã�
     * CFG
     *      0��SPIʹ��λ        0��SPI����      1��SPIʹ��
     *      2��ģʽѡ��λ       0��SPI�ӻ�      1��SPI����
     *      3������λ��ѡ��     0����׼(MSBF)   1����ת(LSBF)
     *      4��ʱ����λѡ��     0����1���ز���  1����2���ز���
     *      5��ʱ�Ӽ���ѡ��     0��֡��SCKΪ0   1��֡��SCKΪ1
     *      7����дģʽʹ��     0����ֹ��д     1��ʹ�ܻ�д
     *      8��SSEL��Ч����ѡ�� 0���͵�ƽ��Ч   1���ߵ�ƽ��Ч
     */
    LPC_SPI0->CFG = (1 << 0) |                                          /* SPIʹ��                      */
                    (1 << 2) |                                          /* ����ģʽ                     */
                    (0 << 3) |                                          /* MSBF                         */
                    (0 << 4) |                                          /* CPHA = 1                     */
                    (0 << 5) |                                          /* CPOL = 1                     */
                    (0 << 7) |                                          /* ��ֹ��д                     */
                    (0 << 8);                                           /* SSEL�͵�ƽ��Ч               */

    /* Initializing Buffer section ----------------------------------------------------------- */
    for ( i = 0; i < DF_BUFF_SIZE; i++ )
    {
        DfBuff[i] = 0 ;
    }

    sta = GetFlashStatus();

    for ( i = 0; i < NUMDATAFLASH; i++ )
    {
        if ( at45Devices[i].id == ( sta & 0x3c ) )
        {
            FlashType = i ;
        }
    }

    BinaryPageFlag = sta & 0x1 ; //0--264BYTE��1--256BYTE

    if ( BinaryPageFlag == 0 )
    {
        PageSize = at45Devices[FlashType].pageSize;
    }
    else
    {
        PageSize = ( at45Devices[FlashType].pageSize >> 8 ) << 8;
    }
}

/*************************************************************
  Function: static void df_wait_busy(void)
  Description: ����ʱ���ػ�ʱ����
  Calls:
  Called By:
  Input:       ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
static void df_wait_busy ( void )
{
    uint32_t i;
    uint8_t sta;
    /* Select the FLASH: Chip Select low */
    CE_Low();
    i = 500000;

    while ( 1 )
    {
        sta = GetFlashStatus();

        if ( ( sta & 0x80 ) )
        {
            break;; /* Busy in progress */
        }
    }

    /* Deselect the FLASH: Chip Select high */
    CE_High();
}

/*************************************************************
  Function: uint32_t GetDfAddress(uint32_t address)
  Description: �������ַת��Ϊҳ��ַ+�ֽڵ�ַ�ĸ�ʽ
  Calls:
  Called By:
  Input:       ��
  Output:      ��
  Return:      ����AT45DBϵ��Ҫ��ĵ�ַ
  Others:      ��
*************************************************************/
uint32_t GetDfAddress ( uint32_t address )
{
    uint32_t dfAddress;

    if ( !BinaryPageFlag )
    {
        dfAddress =
            ( ( address / ( PageSize ) ) << at45Devices[FlashType].pageOffset )
            + ( address % ( PageSize ) );
    }
    else
    {
        dfAddress = address;
    }

    return dfAddress;
}
//�����洢��ָ��ҳ����������(BUFFER1)
static void load_page_to_buffer ( uint32_t page, uint8_t buffer )
{
    CE_Low();

    if ( buffer == DF_READ_BUFFER )
    {
        SendRecv_Start (  MM_PAGE_TO_B1_XFER );
    }
    else
    {
        SendRecv_Start (  MM_PAGE_TO_B2_XFER );
    }

    SendRecv_Byte (  ( uint8_t ) ( page >> 16 ) );
    SendRecv_Byte (  ( uint8_t ) ( page >> 8 ) );
    SendRecv_Stop (  ( uint8_t ) ( page ) );

    CE_High();

    df_wait_busy();
}
//��д����������д�뵽���洢����ָ��ҳ
static void write_page_from_buffer ( uint32_t page, uint8_t buffer )
{
    CE_Low();

    if ( buffer == DF_WRITE_BUFFER )
    {
        SendRecv_Start (  B2_TO_MM_PAGE_PROG_WITH_ERASE );
    }
    else
    {
        SendRecv_Start (  B1_TO_MM_PAGE_PROG_WITH_ERASE );
    }

    SendRecv_Byte (  ( uint8_t ) ( page >> 16 ) );
    SendRecv_Byte (  ( uint8_t ) ( page >> 8 ) );
    SendRecv_Stop (  ( uint8_t ) ( page ) );

    CE_High();

    df_wait_busy();
}
//�Ӷ�������������
void read_buffer ( uint32_t addr, uint8_t *data, uint8_t size )
{
    uint8_t i;

    CE_Low();

    SendRecv_Start (  BUFFER_1_READ );
    SendRecv_Byte (  ( uint8_t ) ( addr >> 16 ) );
    SendRecv_Byte (  ( uint8_t ) ( addr >> 8 ) );
    SendRecv_Byte (  ( uint8_t ) addr );
    //SendRecv_Byte (  0 );

    for ( i = 0; i < size-1; i++ )
    {
        data[i] = SendRecv_Byte (  0 );
    }
    data[i] = SendRecv_Stop (  0 );
    CE_High();
}
//������д��д������
void write_buffer ( uint32_t addr, uint8_t *data, uint8_t size )
{
    uint8_t i;

    CE_Low();

    SendRecv_Start (  BUFFER_2_WRITE );
    SendRecv_Byte (  ( uint8_t ) ( addr >> 16 ) );
    SendRecv_Byte (  ( uint8_t ) ( addr >> 8 ) );
    SendRecv_Byte (  ( uint8_t ) addr );

    for ( i = 0; i < size-1; i++ )
    {
        SendRecv_Byte (  data[i] );
    }
    SendRecv_Stop(  data[i] );
    CE_High();
}

/*************************************************************
  Function: void FlashWrite (uint32_t addr, uint16_t len, uint8_t *buff )
  Description: ������������д��ָ��FLASH��ַ
  Calls:
  Called By:
  Input:       addr--ָ����ַ len--���� buff--��¼�������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void FlashWrite ( uint32_t addr, uint16_t len, uint8_t *buff )
{
    uint32_t address;
    address = GetDfAddress ( addr );
    load_page_to_buffer ( address, DF_WRITE_BUFFER );
    write_buffer ( address, buff, len );
    write_page_from_buffer ( address, DF_WRITE_BUFFER );
}
/*************************************************************
  Function: void FlashRead (uint32_t addr, uint16_t len, uint8_t *buff )
  Description: ��ָ��FLASH��ַ���ݶ���������
  Calls:
  Called By:
  Input:       addr--ָ����ַ len--���� buff--��¼�������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void FlashRead ( uint32_t addr, uint16_t len, uint8_t *buff )
{
    uint32_t address;
    address = GetDfAddress ( addr );
    load_page_to_buffer ( address, DF_READ_BUFFER );
    read_buffer ( address, buff, len );
}


/*************************************************************
  Function: void SaveRecord(uint32_t num,uint8_t *buff)
  Description: �洢��¼��AT45DB081
  Calls:
  Called By:
  Input:       num--��¼���  buff--��¼�������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void SaveRecord ( uint8_t *buff )
{
    uint32_t addr , temp;

    if ( RecordNum >= 500 )
    {
        RecordNum %= 500 ;

        if ( RecordFlag == 0 )
        {
            RecordFlag = 1 ;//��500�����
        }
    }

    //NumPerPage = (PageSize/(264-8*BinaryPageFlag))*DF_RECORD_NUM_PER_PAGE;
    temp = RecordNum / DF_RECORD_NUM_PER_PAGE ;
    addr = temp * ( 264 - 8 * BinaryPageFlag ) ;
    temp = DF_RECORD_BASE + ( RecordNum % DF_RECORD_NUM_PER_PAGE ) * DF_RECORD_LEN ;
    addr += temp ;
    FlashWrite ( addr, DF_RECORD_LEN, buff ); //д���ݵ�������
    RecordNum++;
}

/*************************************************************
  Function: void LoadRecord(uint32_t num,uint8_t *buff)
  Description: ��AT45DB081�еļ�¼
  Calls:
  Called By:
  Input:       num--��¼���  buff--��¼�������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void LoadRecord ( uint32_t num, uint8_t *buff )
{
    int32_t tmp ;
    uint32_t addr, temp;

    if ( RecordNum == 0 )
    {
        return;
    }

    tmp = RecordNum - num ; //ȡ��¼˳��Ϊ����

    if ( tmp < 0 )
    {
        tmp += 500 ;//ѭ������β��ȡ
    }

    temp = tmp / DF_RECORD_NUM_PER_PAGE ;
    addr = temp * ( 264 - 8 * BinaryPageFlag ) ;
    temp = DF_RECORD_BASE + ( tmp % DF_RECORD_NUM_PER_PAGE ) * DF_RECORD_LEN ;
    addr += temp ;
    FlashRead ( addr, DF_RECORD_LEN, buff );
}

/*************************************************************
  Function: void SaveEvent(uint32_t num,uint8_t *buff)
  Description: �洢�¼���AT45DB081
  Calls:
  Called By:
  Input:       num--�¼����  buff--�¼��������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void SaveEvent (uint8_t num, uint8_t *buff,uint8_t len )
{
    __disable_irq(); 
    FlashWrite ( num, len, buff ); //д���ݵ�������
    __enable_irq();
}

/*************************************************************
  Function: void LoadEvent(uint32_t num,uint8_t *buff)
  Description: ��AT45DB081�е��¼�
  Calls:
  Called By:
  Input:       num--�¼����  buff--�¼��������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void LoadEvent ( uint32_t num, uint8_t *buff ,uint8_t len )
{
    FlashRead ( num, len, buff );
}

/*************************************************************
  Function: void SaveAlarm(uint32_t num,uint8_t *buff)
  Description: �洢�����¼���AT45DB081
  Calls:
  Called By:
  Input:       num--�¼����  buff--�¼��������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void SaveAlarm ( uint8_t *buff )
{
    uint32_t addr, temp;

    if ( AlarmNum >= 500 )
    {
        AlarmNum %= 500 ;

        if ( AlarmFlag == 0 )
        {
            AlarmFlag = 1 ;//��500�����
        }
    }

    temp = AlarmNum / DF_ALARM_NUM_PER_PAGE ;
    addr = temp * ( 264 - 8 * BinaryPageFlag ) ;
    temp = DF_ALARM_BASE + ( AlarmNum % DF_ALARM_NUM_PER_PAGE ) * DF_ALARM_LEN ;
    addr += temp ;
    FlashWrite ( addr, DF_ALARM_LEN, buff );
    AlarmNum++;
}

/*************************************************************
  Function: void LoadAlarm(uint32_t num,uint8_t *buff)
  Description: ��AT45DB081�еı����¼�
  Calls:
  Called By:
  Input:       num--�¼����  buff--�¼��������׵�ַ
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void LoadAlarm ( uint32_t num, uint8_t *buff )
{
    int32_t tmp ;
    uint32_t addr, temp;

    if ( AlarmNum == 0 )
    {
        return;
    }

    tmp = AlarmNum - num ; //ȡ��¼˳��Ϊ����

    if ( tmp < 0 )
    {
        tmp += 500 ;//ѭ������β��ȡ
    }

    temp = tmp / DF_ALARM_NUM_PER_PAGE ;
    addr = temp * ( 264 - 8 * BinaryPageFlag ) ;
    temp = DF_ALARM_BASE + ( tmp % DF_ALARM_NUM_PER_PAGE ) * DF_ALARM_LEN ;
    addr += temp ;
    FlashRead ( addr, DF_ALARM_LEN, buff );
}

/*************************************************************
  Function: uint16_t GetRecordNum(void)
  Description: ��ȡ��ǰ��¼����
  Calls:
  Called By:
  Input:       ��
  Output:      ��
  Return:      RecordNum
  Others:      ��
*************************************************************/
uint16_t GetRecordNum ( void )
{
    if ( RecordFlag )
    {
        return 500 ;
    }
    else
    {
        return RecordNum;
    }
}

/*************************************************************
  Function: uint16_t GetEventNum(void)
  Description: ��ȡ��ǰ�¼���¼����
  Calls:
  Called By:
  Input:       ��
  Output:      ��
  Return:      EventNum
  Others:      ��
*************************************************************/
uint16_t GetEventNum ( void )
{
    if ( EventFlag )
    {
        return 500 ;
    }
    else
    {
        return EventNum;
    }
}

/*************************************************************
  Function: uint16_t GetAlarmNum(void)
  Description: ��ȡ��ǰ������¼����
  Calls:
  Called By:
  Input:       ��
  Output:      ��
  Return:      EventNum
  Others:      ��
*************************************************************/
uint16_t GetAlarmNum ( void )
{
    if ( AlarmFlag )
    {
        return 500 ;
    }
    else
    {
        return AlarmNum;
    }

}
void ClearRecord ( void )
{
    AlarmNum = 0 ;
    AlarmFlag = 0 ;
    EventNum = 0 ;
    EventFlag = 0 ;
    RecordNum = 0 ;
    RecordFlag = 0 ;
}
void df161_test ( void )
{
    uint32_t i, j;

    for ( j = 0; j < 10; j++ )
    {
        for ( i = 0; i < 100; i++ )
        {
            DfBuff[i] = i;
        }

        RecordNum = 101 ;
        SaveRecord ( DfBuff );
        LoadRecord ( 1, DfBuff );
    }
}

