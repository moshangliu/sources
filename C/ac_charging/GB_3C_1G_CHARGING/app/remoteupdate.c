

//#define BINCODE_FLASH
#define BINCODE_SPIFLASH



#ifdef BINCODE_FLASH
#include "drv_inc.h"
#include "Bsp_inc.h"
#include "app_inc.h"
#endif

#ifdef BINCODE_SPIFLASH
#include "board.h"
#include "Dio.h"
#include "uart.h"
#endif

#include "gprs_update.h"
#include "crc.h"



#define MAX_TIMEOVERFLOW		10000    // 3s 
#define MAX_PACKET_BINCODE		512    // 64k
#define MAX_REPEATSENDCOUNTER	3


uint8_t *g_txlen;
uint8_t *g_txflag;
uint8_t *g_txbuf;
uint8_t *g_rxbuf;
uint8_t *g_devstate;


//////////////////  add ������
unsigned int  data_length;

uint8_t g_inupdatemodeflag = 0;
uint8_t g_allowupdate = 0;
uint8_t g_resetflag = 0;

uint32_t g_newversion;
uint32_t g_maxdatapacket;
uint32_t g_maxdatapacket_crc;
uint32_t g_currentdatapacket,g_lastdatapacket;

uint32_t g_crc_bincode;

uint32_t g_writeaddress;
uint32_t g_crc_sum;

uint32_t g_writeflash = 0;

unsigned char writebuf[128];
unsigned char readbuf[128];
uint32_t tempdata;
uint32_t src_iap_array_data[64];
uint32_t usersector;
uint32_t usersectoraddress;

uint8_t g_functioncode;
uint8_t g_tempid[4];
	
uint32_t g_returninfo_timeoverflowcounter;
uint8_t  g_repeatsendcounter;
uint8_t  g_sendflag;
uint32_t g_needpacket;


//////////// end  ������

void UpdateInit(uint8_t *txlen, uint8_t *txflag,uint8_t *txbuf,uint8_t *rxbuf,uint8_t *devstate)
{
	g_txlen = txlen;
	g_txflag = txflag;
	g_txbuf = txbuf;
	
	g_rxbuf = rxbuf;
	
	g_devstate = devstate;
}

uint8_t Get_UpdateStates(void)
{
	return g_inupdatemodeflag;
}


/*************************************************************
Function: void GetCurrentTxDataForUpdate ( unsigned int  data_length)
Description:  ��ȡ���� ���� ��Ҫ�ش������Ե� �������� ͬʱ��������
Calls:       ��
Called By:   
Input:       
Output:      ��
Return:      
Others:      ��
*************************************************************/

void GetCurrentTxDataForUpdate ( unsigned int  data_length)
{
	unsigned int i;
	//����֡��ʼ
	g_txbuf[ADDR_FRAMSTART] = 0x68;
	//����֡����
	g_txbuf[ADDR_FUNCODE] = g_functioncode;
	//id
	for(i =ADDR_FACID ;i<ADDR_TRANDIR ;i++)
		g_txbuf[i] = g_tempid[i-ADDR_FACID];		// id huoqu 
	//���䷽�� ���׮ �� ƽ̨
	g_txbuf[ADDR_TRANDIR] = 0x01;
	//���ݳ���
	g_txbuf[ADDR_DATALEN] = data_length>>8;
	g_txbuf[ADDR_DATALEN+1] = data_length&0xff;
	//���ݲ����ڵ��øú���ʱ��׼����

	//crc16 У��
	g_txbuf[ADDR_CRC] = CRC16(&g_txbuf[ADDR_FRAMSTART],ADDR_CRC,Hi);
	g_txbuf[ADDR_CRC+1] = CRC16(&g_txbuf[ADDR_FRAMSTART],ADDR_CRC,Lo);

	//��������׼�� �Ϳ�ʼ
    *g_txlen = ADDR_CRC + 2;//�����ͳ���
	*g_txflag = 1;
	RS232SendMsg(); 
}



void SendUpdateCommand(uint32_t timetick)
{
	unsigned int  data_length;
	//g_repeatsendcounter
	unsigned int temptime = (timetick - g_returninfo_timeoverflowcounter);
	
	if(g_inupdatemodeflag)
	{
		if((g_repeatsendcounter >= MAX_REPEATSENDCOUNTER) && ( temptime > MAX_TIMEOVERFLOW))
			{
				if(g_functioncode == FUNCODE_OUTUPDATEMODE)
					{
						g_repeatsendcounter = 0;
						g_inupdatemodeflag = 0;
						return;
					}
				else
					g_functioncode = FUNCODE_OUTUPDATEMODE;
				
			}
			
		if(g_sendflag || ( temptime > MAX_TIMEOVERFLOW))
			{
				switch ( g_functioncode )
				{
					case FUNCODE_NEWVERSION:
						data_length = 2;
						g_txbuf[ADDR_DATAFILED] = SOFTWARE_VERSION>>8; 	   //����汾 ��λ
						g_txbuf[ADDR_DATAFILED+1] = SOFTWARE_VERSION & 0xff;  //����汾��λ
						break;
					case FUNCODE_HANDPREPARE:
						data_length = 2;
						g_txbuf[ADDR_DATAFILED] = 0x00; 	  
						g_txbuf[ADDR_DATAFILED+1] = 0x00;  
						break;
					case FUNCODE_DATATRANSLATE:
						data_length = 2;
						g_txbuf[ADDR_DATAFILED] = (g_needpacket>>8) &0xff; 	  
						g_txbuf[ADDR_DATAFILED+1] = g_needpacket & 0xff;  
						break;
					case FUNCODE_UPDATECONFIRM:
						data_length = 6;
						g_txbuf[ADDR_DATAFILED] = (g_needpacket>>8)&0xff; 	  
						g_txbuf[ADDR_DATAFILED+1] = g_needpacket & 0xff; 
						g_txbuf[ADDR_DATAFILED+2] = (g_crc_sum>>24)&0xff;
						g_txbuf[ADDR_DATAFILED+3] = (g_crc_sum>>16)&0xff;
						g_txbuf[ADDR_DATAFILED+4] = (g_crc_sum>>8)&0xff;
						g_txbuf[ADDR_DATAFILED+5] = (g_crc_sum>>0)&0xff;
						
						break;
					case FUNCODE_OUTUPDATEMODE:
						data_length = 2;
						g_txbuf[ADDR_DATAFILED] = 0x00; 	  
						g_txbuf[ADDR_DATAFILED+1] = 0x00;  
						break;	
					default:
						break;
				}
				
			GetCurrentTxDataForUpdate(data_length);

				if(g_sendflag)
					{
					g_sendflag = 0;
					g_repeatsendcounter = 0;
					}
				else if(temptime >MAX_TIMEOVERFLOW)
					{
						g_repeatsendcounter++;
						
					}
				g_returninfo_timeoverflowcounter = timetick;
			}
	}
}

/*************************************************************
Function: void DealUpdate(void)
Description:  ������������ ���͹���������
Calls:       ��
Called By:   
Input:       
Output:      ��
Return:      
Others:      ��
*************************************************************/

void DealUpdate_Flash(uint8_t len,uint32_t timetick)
{
	unsigned int  data_length;
	unsigned int  i,j;
	
	uint8_t *temp;
	uint8_t crc16_h,crc16_l;

	
	data_length = ( g_rxbuf[ADDR_DATALEN] << 8 ) + g_rxbuf[ADDR_DATALEN+1];
	if( len == ( data_length + 11 ) )
	{
		//���� CRC У�� 
		#ifdef BINCODE_FLASH
		temp = g_rxbuf;
		crc16_h = CRC16(&temp[ADDR_FRAMSTART],ADDR_CRC,Hi);
		temp = g_rxbuf;
		crc16_l = CRC16(&temp[ADDR_FRAMSTART],ADDR_CRC,Lo);
		#endif
		#ifdef BINCODE_SPIFLASH
		temp = g_rxbuf;
		crc16_l = CRC16(&temp[ADDR_FRAMSTART],ADDR_CRC,Hi);
		temp = g_rxbuf;
		crc16_h = CRC16(&temp[ADDR_FRAMSTART],ADDR_CRC,Lo);
		#endif
		if(( crc16_h == g_rxbuf[ADDR_CRC]) && ( crc16_l == g_rxbuf[ADDR_CRC+1]))	// 16λУ��
		{
			//�� ���������� ���� ����
			switch ( g_rxbuf[ADDR_FUNCODE] )
			{
				case FUNCODE_GETINUPDATEMODE:		//�������ģʽdd	
						
						if(*g_devstate) // ���ڳ��	���ܽ��� ����ģʽ
							{
							data_length = 2;
							g_inupdatemodeflag = 0;
							g_txbuf[ADDR_DATAFILED] = 0x00;
							g_txbuf[ADDR_DATAFILED+1] = 0x00;
							GetCurrentTxDataForUpdate(data_length);
							}
						else
							{
							for(i =ADDR_FACID ;i<ADDR_TRANDIR ;i++)
								g_tempid[i-ADDR_FACID] = g_rxbuf[i];		// id huoqu 

								g_sendflag = 1;
								g_returninfo_timeoverflowcounter = timetick;
								g_functioncode = FUNCODE_NEWVERSION;

								g_needpacket = 1;
								
								g_inupdatemodeflag = 1;
								g_writeaddress = BACK_PROGRAM_SPIFLASH;
								g_crc_sum = 0;
								g_lastdatapacket = 0;
								g_writeflash = 0;
							}
					break;
				case FUNCODE_NEWVERSION:		//�·��°汾������汾��
					
					g_newversion = (g_rxbuf[ADDR_DATAFILED+2] << 8) +g_rxbuf[ADDR_DATAFILED+3];
		
					if((g_newversion > SOFTWARE_VERSION) && g_rxbuf[ADDR_DATAFILED])
						{
						g_allowupdate = 1;
						
						g_sendflag = 1;
						g_returninfo_timeoverflowcounter = timetick;
						g_functioncode = FUNCODE_HANDPREPARE;
						}
					else
						{
						g_sendflag = 1;
						g_returninfo_timeoverflowcounter = timetick;
						g_functioncode = FUNCODE_OUTUPDATEMODE;
						
						g_allowupdate = 0;
						}
					
					break;
				case FUNCODE_HANDPREPARE:		//����׼��
					data_length = 4;
					g_maxdatapacket = (g_rxbuf[ADDR_DATAFILED] << 8) +g_rxbuf[ADDR_DATAFILED+1];
					
					if((g_maxdatapacket>0) && (g_maxdatapacket <= MAX_PACKET_BINCODE))
						{
						#ifdef BINCODE_FLASH
						j = g_maxdatapacket*128 / 32768 + 1;
						__disable_irq();
						for(i=0;i<j;i++)   //�������� ����
						{
							Chip_IAP_PreSectorForReadWrite(IAP_BACK_START_SECTOR+i, IAP_BACK_START_SECTOR+i);
							Chip_IAP_EraseSector(IAP_BACK_START_SECTOR+i, IAP_BACK_START_SECTOR+i);
						}
						//��־λ ���� ����
						Chip_IAP_PreSectorForReadWrite(IAP_BACKFLAG_START_SECTOR, IAP_BACKFLAG_START_SECTOR);
						Chip_IAP_EraseSector(IAP_BACKFLAG_START_SECTOR, IAP_BACKFLAG_START_SECTOR);
						__enable_irq();
						#endif
						g_sendflag = 1;
						g_returninfo_timeoverflowcounter = timetick;
						g_functioncode = FUNCODE_DATATRANSLATE;
						}
					else
						{
						g_sendflag = 1;
						g_returninfo_timeoverflowcounter = timetick;
						g_functioncode = FUNCODE_OUTUPDATEMODE;
						}
					break;
				case FUNCODE_DATATRANSLATE: 	//�������ݵĴ���
					
					g_currentdatapacket = (g_rxbuf[ADDR_DATAFILED] << 8) +g_rxbuf[ADDR_DATAFILED+1];
					if((g_currentdatapacket == (g_lastdatapacket + 1)) && (g_currentdatapacket == g_needpacket))
					{
						#ifdef BINCODE_FLASH
						temp = (uint8_t *)&tempdata;
						if(g_writeflash%2)
							j =32;
						else
							j = 0;
						
						for(i=0;i<128;i+=4)
							{
								temp[0] = g_rxbuf[i+2 + ADDR_DATAFILED];
								temp[1] = g_rxbuf[i+3 + ADDR_DATAFILED];
								temp[2] = g_rxbuf[i+4 + ADDR_DATAFILED];
								temp[3] = g_rxbuf[i+5 + ADDR_DATAFILED];
								
								src_iap_array_data[j++] = tempdata;
								g_crc_sum += tempdata;
							}
						g_writeflash++;
						if((g_writeflash%2) == 0)
						{
							g_writeflash = 0;
							//memcpy(writebuf,&g_rxbuf[ADDR_DATAFILED+2],128);
							__disable_irq();
							//FlashWrite(g_writeaddress,128,writebuf);  // addr ��ַ��Ҫ����
							//FlashRead(g_writeaddress,128,readbuf);
							usersector = (IAP_BACK_START_SECTOR + ((g_currentdatapacket - 2)*128)/32768);
							usersectoraddress = IAP_BACK_START_ADDRESS +(g_currentdatapacket - 2)*128;
							Chip_IAP_PreSectorForReadWrite(usersector, usersector);
							Chip_IAP_CopyRamToFlash(usersectoraddress, src_iap_array_data, 256);
							__enable_irq();
						}
						#endif
						#ifdef BINCODE_SPIFLASH
						temp = (uint8_t *)&tempdata;

						for(i=0;i<128;i+=4)
							{
								temp[0] = g_rxbuf[i+2 + ADDR_DATAFILED];
								temp[1] = g_rxbuf[i+3 + ADDR_DATAFILED];
								temp[2] = g_rxbuf[i+4 + ADDR_DATAFILED];
								temp[3] = g_rxbuf[i+5 + ADDR_DATAFILED];
								
								
								g_crc_sum += tempdata;
							}
						
						memcpy(writebuf,&g_rxbuf[ADDR_DATAFILED+2],128);
						__disable_irq();
						FlashWrite(g_writeaddress,128,writebuf);  // addr ��ַ��Ҫ����
						//FlashRead(g_writeaddress,128,readbuf);
						__enable_irq();
						
						if((g_writeflash%2) == 0)
							g_writeaddress += 128;
						else
							g_writeaddress += (128 + 8);
						
						g_writeflash++;
						#endif
						g_lastdatapacket = g_currentdatapacket;
						
						g_sendflag = 1;
						g_returninfo_timeoverflowcounter = timetick;
						g_functioncode = FUNCODE_DATATRANSLATE;
						
						if(g_needpacket < g_maxdatapacket)
							g_needpacket++;
						else
							g_functioncode = FUNCODE_UPDATECONFIRM;
							
					}
					else
					{
						g_sendflag = 1;
						g_returninfo_timeoverflowcounter = timetick;
						g_functioncode = FUNCODE_OUTUPDATEMODE;
					}
					break;
				case FUNCODE_UPDATECONFIRM: 	//��������
					
					g_maxdatapacket_crc = (g_rxbuf[ADDR_DATAFILED] << 8) +g_rxbuf[ADDR_DATAFILED+1];

					g_crc_bincode = (g_rxbuf[ADDR_DATAFILED+2]<<24) + (g_rxbuf[ADDR_DATAFILED+3]<<16) + 
									(g_rxbuf[ADDR_DATAFILED+4]<<8) + g_rxbuf[ADDR_DATAFILED+5];
					
				
					if((g_maxdatapacket_crc == g_maxdatapacket) && (g_crc_sum == g_crc_bincode)
						&& (g_maxdatapacket_crc == g_needpacket) &&g_rxbuf[ADDR_DATAFILED+6])
						{
						#ifdef BINCODE_FLASH
						__disable_irq();
						if((g_writeflash%2) == 1)
						{
							usersector = (IAP_BACK_START_SECTOR + ((g_currentdatapacket - 1)*128)/32768);
							usersectoraddress = IAP_BACK_START_ADDRESS +(g_currentdatapacket - 1)*128;
							Chip_IAP_PreSectorForReadWrite(usersector, usersector);
							Chip_IAP_CopyRamToFlash(usersectoraddress, src_iap_array_data, 256);
						}

						src_iap_array_data[0] = UPDATE_FLGE;
						src_iap_array_data[1] = g_maxdatapacket;
						src_iap_array_data[2] = g_crc_bincode;
						//FlashWrite(BACK_LEN_CRC_SPIFLASH,12,writebuf);	// addr ��ַ��Ҫ����
						//FlashRead(BACK_LEN_CRC_SPIFLASH,12,readbuf);
						Chip_IAP_PreSectorForReadWrite(IAP_BACKFLAG_START_SECTOR, IAP_BACKFLAG_START_SECTOR);
						Chip_IAP_CopyRamToFlash(IAP_BACKFLAG_START_ADDRESS, src_iap_array_data, 256);
						__enable_irq();
						#endif
						#ifdef BINCODE_SPIFLASH
						tempdata= UPDATE_FLGE;
						writebuf[0] = tempdata&0xff;
						writebuf[1] = (tempdata>>8)&0xff;
						writebuf[2] = (tempdata>>16)&0xff;
						writebuf[3] = (tempdata>>24)&0xff;
						
						writebuf[4] = g_maxdatapacket&0xff;
						writebuf[5] = (g_maxdatapacket>>8)&0xff;
						writebuf[6] = (g_maxdatapacket>>16)&0xff;
						writebuf[7] = (g_maxdatapacket>>24)&0xff;
						
						writebuf[8] = g_crc_bincode&0xff;
						writebuf[9] = (g_crc_bincode>>8)&0xff;
						writebuf[10] = (g_crc_bincode>>16)&0xff;
						writebuf[11] = (g_crc_bincode>>24)&0xff;
						
						__disable_irq();
						FlashWrite(BACK_LEN_CRC_SPIFLASH,12,writebuf);  // addr ��ַ��Ҫ����
						//FlashRead(BACK_LEN_CRC_SPIFLASH,12,readbuf);
						__enable_irq();
						
						#endif
						g_resetflag = 1;

						if(g_resetflag)
						{
							while(*g_txflag)    //�ȴ� dma usart tx �������� �������
								{}
							NVIC_SystemReset();		//ϵͳ��λ
						}
						}
					else
						{
							g_sendflag = 1;
							g_returninfo_timeoverflowcounter = timetick;
							g_functioncode = FUNCODE_OUTUPDATEMODE;
						}
						
					break;
				case FUNCODE_OUTUPDATEMODE:		//�˳�����ģʽ

							g_inupdatemodeflag = 0;
							g_crc_sum = 0;
							g_lastdatapacket = 0;
							g_allowupdate = 0;
						
					break;

				default:
					break;
			}
		}
	}
}



#if 0
/*************************************************************
Function: void GetCurrentTxDataForUpdate ( unsigned int  data_length)
Description:  ��ȡ���� ���� ��Ҫ�ش������Ե� �������� ͬʱ��������
Calls:       ��
Called By:   
Input:       
Output:      ��
Return:      
Others:      ��
*************************************************************/
void GetCurrentTxDataForUpdate ( unsigned int  data_length)
{
	unsigned int i;

	for(i =0 ;i<ADDR_TRANDIR ;i++)
		RS232Tx.Buff[i] = RS232Rx.Buff[i];

	RS232Tx.Buff[ADDR_TRANDIR] = 0x01;

	RS232Tx.Buff[ADDR_CRC] = CRC16(&RS232Tx.Buff[ADDR_DATAFILED],data_length,Hi);
	RS232Tx.Buff[ADDR_CRC+1] = CRC16(&RS232Tx.Buff[ADDR_DATAFILED],data_length,Lo);

    RS232Tx.Len = ADDR_CRC + 2;//�����ͳ���
	RS232Tx.Flag = 1;
	RS232SendMsg(); 
}

/*************************************************************
Function: void DealUpdate(void)
Description:  ������������ ���͹���������
Calls:       ��
Called By:   
Input:       
Output:      ��
Return:      
Others:      ��
*************************************************************/

void DealUpdate(uint8_t len)
{
	unsigned int  data_length;
	unsigned int  i,j;
	
	uint8_t *temp;
	uint8_t crc16_h,crc16_l;
	data_length = ( RS232Rx.Buff[ADDR_DATALEN] << 8 ) + RS232Rx.Buff[ADDR_DATALEN+1];
	if( len == ( data_length + 11 ) )
	{
		//���� CRC У�� 
		crc16_h = CRC16(&RS232Rx.Buff[ADDR_FRAMSTART],ADDR_CRC,Hi);
		crc16_l = CRC16(&RS232Rx.Buff[ADDR_FRAMSTART],ADDR_CRC,Lo);
		
		if(( crc16_h == RS232Rx.Buff[ADDR_CRC]) && ( crc16_l == RS232Rx.Buff[ADDR_CRC+1]))	// 16λУ��
		{
			//�� ���������� ���� ����
			switch ( RS232Rx.Buff[ADDR_FUNCODE] )
			{
				case FUNCODE_GETINUPDATEMODE:		//�������ģʽdd	
						data_length = 2;
						RS232Tx.Buff[ADDR_DATALEN] = 0x00;
						RS232Tx.Buff[ADDR_DATALEN+1] = 0x02;

						if(Dev_Status) // ���ڳ��	���ܽ��� ����ģʽ
							{
							g_inupdatemodeflag = 0;
							RS232Tx.Buff[ADDR_DATAFILED] = 0x00;
							}
						else
							{
							g_inupdatemodeflag = 1;
							g_writeaddress = BACK_PROGRAM_SPIFLASH;
							g_crc_sum = 0;
							g_lastdatapacket = 0;
							RS232Tx.Buff[ADDR_DATAFILED] = 0x01;
							}
						RS232Tx.Buff[ADDR_DATAFILED+1] = 0x00;
						GetCurrentTxDataForUpdate(data_length);
					break;
				case FUNCODE_CURRENTVERSION:		//��ȡ��ǰ�汾��
					data_length = 2;
					RS232Tx.Buff[ADDR_DATALEN] = 0x00;
					RS232Tx.Buff[ADDR_DATALEN+1] = 0x02;
					RS232Tx.Buff[ADDR_DATAFILED] = SOFTWARE_VERSION>>8; 	   //����汾 ��λ
					RS232Tx.Buff[ADDR_DATAFILED+1] = SOFTWARE_VERSION & 0xff;  //����汾��λ
					GetCurrentTxDataForUpdate(data_length);
					break;
				case FUNCODE_NEWVERSION:		//�·��°汾������汾��
					data_length = 4;
					g_newversion = (RS232Rx.Buff[ADDR_DATAFILED] << 8) +RS232Rx.Buff[ADDR_DATAFILED+1];
					RS232Tx.Buff[ADDR_DATALEN] = 0x00;
					RS232Tx.Buff[ADDR_DATALEN+1] = 0x04;
					RS232Tx.Buff[ADDR_DATAFILED] = RS232Rx.Buff[ADDR_DATAFILED];
					RS232Tx.Buff[ADDR_DATAFILED+1] = RS232Rx.Buff[ADDR_DATAFILED+1];
					
					if(g_newversion > SOFTWARE_VERSION)
						{
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x01;	 //����汾�·��ɹ�
						g_allowupdate = 1;
						}
					else
						{
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x00;	//����汾�·�ʧ��
						g_allowupdate = 0;
						}
					
					RS232Tx.Buff[ADDR_DATAFILED+3] = 0x00;
					GetCurrentTxDataForUpdate(data_length);
					break;
				case FUNCODE_HANDPREPARE:		//����׼��
					data_length = 4;
					g_maxdatapacket = (RS232Rx.Buff[ADDR_DATAFILED] << 8) +RS232Rx.Buff[ADDR_DATAFILED+1];
					RS232Tx.Buff[ADDR_DATALEN] = 0x00;
					RS232Tx.Buff[ADDR_DATALEN+1] = 0x04;
					RS232Tx.Buff[ADDR_DATAFILED] = RS232Rx.Buff[ADDR_DATAFILED];
					RS232Tx.Buff[ADDR_DATAFILED+1] = RS232Rx.Buff[ADDR_DATAFILED+1];
					if(g_inupdatemodeflag && g_allowupdate)
						{
						j = g_maxdatapacket*128 / 32768 + 1;
						for(i=0;i<j;i++)   //�������� ����
						{
							Chip_IAP_PreSectorForReadWrite(IAP_BACK_START_SECTOR+i, IAP_BACK_START_SECTOR+i);
							Chip_IAP_EraseSector(IAP_BACK_START_SECTOR+i, IAP_BACK_START_SECTOR+i);
						}
						//��־λ ���� ����
						Chip_IAP_PreSectorForReadWrite(IAP_BACKFLAG_START_SECTOR, IAP_BACKFLAG_START_SECTOR);
						Chip_IAP_EraseSector(IAP_BACKFLAG_START_SECTOR, IAP_BACKFLAG_START_SECTOR);
							
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x01;	 //���Ը���
						}
					else
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x00;	//�����Ը���
					
					RS232Tx.Buff[ADDR_DATAFILED+3] = 0x00;
					GetCurrentTxDataForUpdate(data_length);
					break;
				case FUNCODE_DATATRANSLATE: 	//�������ݵĴ���
					data_length = 4;
					
					g_currentdatapacket = (RS232Rx.Buff[ADDR_DATAFILED] << 8) +RS232Rx.Buff[ADDR_DATAFILED+1];
					if(g_currentdatapacket == (g_lastdatapacket + 1))
					{
						temp = (uint8_t *)&tempdata;
						if(g_writeflash%2)
							j =32;
						else
							j = 0;
						
						for(i=0;i<128;i+=4)
							{
								temp[0] = RS232Rx.Buff[i+2 + ADDR_DATAFILED];
								temp[1] = RS232Rx.Buff[i+3 + ADDR_DATAFILED];
								temp[2] = RS232Rx.Buff[i+4 + ADDR_DATAFILED];
								temp[3] = RS232Rx.Buff[i+5 + ADDR_DATAFILED];
								
								src_iap_array_data[j++] = tempdata;
								g_crc_sum += tempdata;
							}
						g_writeflash++;
						if((g_writeflash%2) == 0)
						{
							g_writeflash = 0;
							//memcpy(writebuf,&RS232Rx.Buff[ADDR_DATAFILED+2],128);
							__disable_irq();
							//FlashWrite(g_writeaddress,128,writebuf);  // addr ��ַ��Ҫ����
							//FlashRead(g_writeaddress,128,readbuf);
							usersector = (IAP_BACK_START_SECTOR + ((g_currentdatapacket - 2)*128)/32768);
							usersectoraddress = IAP_BACK_START_ADDRESS +(g_currentdatapacket - 2)*128;
							Chip_IAP_PreSectorForReadWrite(usersector, usersector);
							Chip_IAP_CopyRamToFlash(usersectoraddress, src_iap_array_data, 256);
							__enable_irq();
						}
						//g_writeaddress += 128;
						//for(i=0;i<128;i++)
							{
						//		if(readbuf[i] != writebuf[i])
									{
						//			readbuf[i] = writebuf[i];
						//			break;
									}
							}
						
						//g_writeaddress += 128;
						g_lastdatapacket = g_currentdatapacket;
					}
					RS232Tx.Buff[ADDR_DATALEN] = 0x00;
					RS232Tx.Buff[ADDR_DATALEN+1] = 0x04;
					RS232Tx.Buff[ADDR_DATAFILED] = RS232Rx.Buff[ADDR_DATAFILED];
					RS232Tx.Buff[ADDR_DATAFILED+1] = RS232Rx.Buff[ADDR_DATAFILED+1];
					
					if(g_currentdatapacket == g_lastdatapacket)
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x01;	 //����ɹ�
					else
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x00;	//����ʧ��
					
					RS232Tx.Buff[ADDR_DATAFILED+3] = 0x00;
					GetCurrentTxDataForUpdate(data_length);

					break;
				case FUNCODE_UPDATECONFIRM: 	//��������
					data_length = 4;
					
					g_maxdatapacket_crc = (RS232Rx.Buff[ADDR_DATAFILED] << 8) +RS232Rx.Buff[ADDR_DATAFILED+1];

					g_crc_bincode = (RS232Rx.Buff[ADDR_DATAFILED+2]<<24) + (RS232Rx.Buff[ADDR_DATAFILED+3]<<16) + 
									(RS232Rx.Buff[ADDR_DATAFILED+4]<<8) + RS232Rx.Buff[ADDR_DATAFILED+5];
					
					RS232Tx.Buff[ADDR_DATALEN] = 0x00;
					RS232Tx.Buff[ADDR_DATALEN+1] = 0x04;
					RS232Tx.Buff[ADDR_DATAFILED] = RS232Rx.Buff[ADDR_DATAFILED];
					RS232Tx.Buff[ADDR_DATAFILED+1] = RS232Rx.Buff[ADDR_DATAFILED+1];
					
					if((g_maxdatapacket_crc == g_maxdatapacket) && (g_crc_sum == g_crc_bincode))
						{

						
						
						__disable_irq();
						if((g_writeflash%2) == 1)
						{
							usersector = (IAP_BACK_START_SECTOR + ((g_currentdatapacket - 1)*128)/32768);
							usersectoraddress = IAP_BACK_START_ADDRESS +(g_currentdatapacket - 1)*128;
							Chip_IAP_PreSectorForReadWrite(usersector, usersector);
							Chip_IAP_CopyRamToFlash(usersectoraddress, src_iap_array_data, 256);
						}

						src_iap_array_data[0] = UPDATE_FLGE;
						src_iap_array_data[1] = g_maxdatapacket;
						src_iap_array_data[2] = g_crc_bincode;
						//FlashWrite(BACK_LEN_CRC_SPIFLASH,12,writebuf);	// addr ��ַ��Ҫ����
						//FlashRead(BACK_LEN_CRC_SPIFLASH,12,readbuf);
						Chip_IAP_PreSectorForReadWrite(IAP_BACKFLAG_START_SECTOR, IAP_BACKFLAG_START_SECTOR);
						Chip_IAP_CopyRamToFlash(IAP_BACKFLAG_START_ADDRESS, src_iap_array_data, 256);
						__enable_irq();
						//g_writeaddress += 128;
						//for(i=0;i<12;i++)
							{
						//		if(readbuf[i] != writebuf[i])
									{
						//			readbuf[i] = writebuf[i];
						//			break;
									}
							}
						
						g_resetflag = 1;
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x01;	 //У��ɹ�
						}
					else
						RS232Tx.Buff[ADDR_DATAFILED+2] = 0x00;	//У��ʧ��
					
					RS232Tx.Buff[ADDR_DATAFILED+3] = 0x00;
					GetCurrentTxDataForUpdate(data_length);

					if(g_resetflag)
						{
							while(RS232Tx.Flag)    //�ȴ� dma usart tx �������� �������
								{}
							//NVIC_SystemReset();		//ϵͳ��λ
						}
					break;

				default:
					break;
			}
		}
	}
}
#endif

