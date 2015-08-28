
#ifndef GPRS_UPDATE_H__
#define GPRS_UPDATE_H__



#define SOFTWARE_VERSION		125

#define UPDATE_FLGE		0x5566aabb

#define TRANDIR_PLATFORMTOPILE		0		//���䷽�� ƽ̨��׮
#define TRANDIR_PILETOPLATFORM		1		//���䷽��׮��ƽ̨

#define 	BACK_LEN_CRC_SIZE		528
#define 	BACK_LEN_CRC_SPIFLASH	0x0
#define 	BACK_PROGRAM_SPIFLASH	(BACK_LEN_CRC_SPIFLASH + BACK_LEN_CRC_SIZE)

#define SIZE_FRAMSTART		1
#define SIZE_FUNCODE		1
#define SIZE_FACID			4
#define SIZE_TRANDIR		1
#define SIZE_DATALEN		2
#define SIZE_DATAFILED		data_length
#define SIZE_CRC			2


#define ADDR_FRAMSTART		0
#define ADDR_FUNCODE		(ADDR_FRAMSTART + SIZE_FRAMSTART)
#define ADDR_FACID			(ADDR_FUNCODE + SIZE_FUNCODE)
#define ADDR_TRANDIR		(ADDR_FACID + SIZE_FACID)
#define ADDR_DATALEN		(ADDR_TRANDIR + SIZE_TRANDIR)
#define ADDR_DATAFILED		(ADDR_DATALEN + SIZE_DATALEN)
#define ADDR_CRC			(ADDR_DATAFILED + SIZE_DATAFILED)

#define FUNCODE_GETINUPDATEMODE		0xf0
//#define FUNCODE_CURRENTVERSION		0xf1
#define FUNCODE_NEWVERSION			0xf1
#define FUNCODE_HANDPREPARE			0xf2
#define FUNCODE_DATATRANSLATE		0xf3
#define FUNCODE_UPDATECONFIRM		0xf4
#define FUNCODE_OUTUPDATEMODE		0xf5

#define SECTOR_SIZE             4096
#define SECTOR_SIZE_32			0x8000
/* LAST SECTOR */
#define IAP_START_SECTOR		16  //0x10000
#define IAP_LAST_SECTOR         16

#define IAP_BACK_START_SECTOR	20  //0x30000
#define IAP_BACKFLAG_START_SECTOR 24 //0x50000

#define IAP_START_ADDRESS				0x10000
#define IAP_BACK_START_ADDRESS			0x30000
#define IAP_BACKFLAG_START_ADDRESS		0x50000


#endif