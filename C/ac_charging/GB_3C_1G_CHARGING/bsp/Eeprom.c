
#include "board.h"
#include "eeprom.h"

//extern PARACONFIG g_sParaConfig;
uint8_t ParaBuff[16];
#define PARA_ADDR  0
#define PARA_LEN   (sizeof(g_sParaConfig)+1)



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
	/* Size of each sector */
#define SECTOR_SIZE             1024
	
	/* LAST SECTOR */
#define IAP_START_SECTOR		31   

uint8_t LoadParameter ( void )
{
    uint8_t *ptr, *ptr1, len, i, cs;

    cs = 0 ;
    //LoadEvent ( 0, ParaBuff, PARA_LEN );
    
    for(i=0;i<PARA_LEN;i++)
		ParaBuff[i] = *(unsigned char *)(IAP_START_SECTOR * SECTOR_SIZE + i);
		
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
	uint32_t write[16];

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
    //SaveEvent ( 0, ParaBuff, PARA_LEN );

	ptr = ( uint8_t * )write;

	for(i=0;i<PARA_LEN;i++)
		ptr[i] = ParaBuff[i];
	
	for(i=PARA_LEN;i<64;i++)
		ptr[i] = 0xff;
	
	//for(i=0;i<16;i++)
	//	write[i] = 0x56575859 + i;
			
	__disable_irq();
	Chip_IAP_PreSectorForReadWrite(IAP_START_SECTOR, IAP_START_SECTOR);
	Chip_IAP_EraseSector(IAP_START_SECTOR, IAP_START_SECTOR);
	Chip_IAP_PreSectorForReadWrite(IAP_START_SECTOR, IAP_START_SECTOR);
	Chip_IAP_CopyRamToFlash(IAP_START_SECTOR * SECTOR_SIZE, write, 64);
	__enable_irq();
}

