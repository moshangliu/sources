#ifndef __CRC_H__
#define __CRC_H__

#define  Hi  0                              //����ѡ��CRC��������ĸߵ�λ
#define  Lo  1

#include "board.h"

uint8_t CRC16( uint8_t * puchMsg,short usDataLen,uint8_t byte_flag);        // �õ�CRC�ĸ��ֽ� (byte_flag == 1)

#endif
