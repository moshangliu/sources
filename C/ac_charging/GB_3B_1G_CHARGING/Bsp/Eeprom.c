
#include "drv_inc.h"
#include "app_inc.h"
#include "bsp_inc.h"

uint8_t ParaBuff[1024];
#define PARA_ADDR  0
#define PARA_LEN   (sizeof(g_sParaConfig)+1)

/*************************************************************
  Function: void EepromWrite(uint16_t addr,uint8_t *buff,uint16_t len)
  Description: д���������ݵ�EEPROM
  Calls:       EEPROM_Write
  Called By:
  Input:       addr--��ַ����Χ0-4095 buff--������ָ��  len--����
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void EepromWrite ( uint16_t addr, uint8_t *buff, uint16_t len )
{
    uint16_t PageOffset, PageAddr;
    PageOffset = addr & 0x3f ;
    PageAddr = ( addr >> 6 ) & 0x3f ;
    EEPROM_Write ( PageOffset, PageAddr, ( void * ) buff, MODE_8_BIT, len );
}

/*************************************************************
  Function: void EepromRead(uint16_t addr,void *buff,uint16_t len)
  Description: ��EEPROM���ݵ�������
  Calls:       EEPROM_Read
  Called By:
  Input:       addr--��ַ����Χ0-4095 buff--������ָ��  len--����
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void EepromRead ( uint16_t addr, uint8_t *buff, uint16_t len )
{
    uint16_t PageOffset, PageAddr;
    PageOffset = addr & 0x3f ;
    PageAddr = ( addr >> 6 ) & 0x3f ;
    EEPROM_Read ( PageOffset, PageAddr, ( void * ) buff, MODE_8_BIT, len );
}

uint8_t GetChkSum ( uint8_t *buff, uint16_t len )
{
    uint8_t cs;
    uint16_t  i;
    cs = 0 ;

    for ( i = 0; i < len; i++ )
    {
        cs += *buff++ ;
    }

    return cs ;
}
/*************************************************************
  Function: void Byte4ToPara(uint32_t *dat,uint8 *ptr)
  Description: ��4���ֽ����Ϊ32Ϊ�Ĳ���
  Calls:
  Called By:
  Input:       dat--32λ���� ptr--4�ֽڻ�����ָ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void Byte4ToPara ( uint32_t *dat, uint8_t *ptr )
{
    uint32_t tmp ;
    tmp = 0 ;
    tmp = *ptr ;
    tmp <<= 8 ;
    tmp |= * ( ptr + 1 );
    tmp <<= 8 ;
    tmp |= * ( ptr + 2 );
    tmp <<= 8 ;
    tmp |= * ( ptr + 3 );
    *dat = tmp ;
}

/*************************************************************
  Function: void ParaToByte4(uint32_t dat,uint8 *ptr)
  Description: ���������Ϊ4���ֽ�
  Calls:
  Called By:
  Input:       dat--32λ���� ptr--4�ֽڻ�����ָ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void ParaToByte4 ( uint32_t dat, uint8_t *ptr )
{
    uint32_t tmp ;
    tmp = dat ;
    * ( ptr + 3 ) = tmp ;
    tmp >>= 8 ;
    * ( ptr + 2 ) = tmp ;
    tmp >>= 8 ;
    * ( ptr + 1 ) = tmp ;
    tmp >>= 8 ;
    * ( ptr + 0 ) = tmp ;
}
/*************************************************************
  Function: uint8_t LoadParameter(void)
  Description: ��EEPROM�ж�����
  Calls:
  Called By:   �����仯��Ҫ����ʱ
  Input:       ��
  Output:      ��
  Return:      1--У��� 0--��ȡ����
  Others:      ��
*************************************************************/
uint8_t LoadParameter ( void )
{
    uint8_t *ptr, *ptr1, len, i, cs;

    cs = 0 ;
    EepromRead ( 0, ParaBuff, PARA_LEN );
    cs = GetChkSum ( ParaBuff, PARA_LEN - 1 );
    cs += 0x33;
    
    if ( cs == ParaBuff[PARA_LEN-1] )
    {
        ;
    }
    else
    {
        //����������
        return 1 ;
    }

    ptr = ( uint8_t * ) ( &g_sParaConfig ) ;
    ptr1 = ParaBuff ;
    len = sizeof ( g_sParaConfig ); //ȡ���ò�����������

    for ( i = 0; i < len; i++ )
    {
        *ptr++ = *ptr1++ ;
    }

    return 0 ;
}

/*************************************************************
  Function: void SaveParameter(void)
  Description: �������в�����EEPROM��ͬʱ����У���
  Calls:
  Called By:   �����仯��Ҫ����ʱ
  Input:       ��
  Output:      ��
  Return:      ��
  Others:      ��
*************************************************************/
void SaveParameter ( void )
{
    uint8_t *ptr, *ptr1, len, i, cs;

    cs = 0 ;
    ptr = ( uint8_t * ) ( &g_sParaConfig ) ;
    ptr1 = ParaBuff ;
    len = sizeof ( g_sParaConfig ); //ȡ���ò����������� 22�ֽ�

    for ( i = 0; i < len; i++ )
    {
        *ptr1++ = *ptr++ ;
    }
    
    cs = GetChkSum ( ParaBuff, PARA_LEN - 1 );
    *ptr1 = cs + 0x33 ;
    len += 1 ;
    EepromWrite ( 0, ParaBuff, PARA_LEN );
}


