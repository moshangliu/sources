#ifndef __EEPROM_H__
#define __EEPROM_H__

void EepromWrite ( uint16_t addr, uint8_t *buff, uint16_t len ); //  д���������ݵ�EEPROM
void EepromWrite ( uint16_t addr, uint8_t *buff, uint16_t len ); // ��EEPROM���ݵ�������
uint8_t LoadParameter ( void ) ; //������Ĳ���
void SaveParameter ( void ); //�������
uint8_t LoadAddress ( void )  ; //������ĵ�ַ��Ϣ
void SaveAddress ( void )  ;//�����ַ��Ϣ

uint8_t LoadEthernet ( void );
uint8_t LoadPwrDownPara ( void );
void SavePwrDownPara ( void );
void SaveEthernet ( void );
void Byte4ToPara ( uint32_t *dat, uint8_t *ptr );
void ParaToByte4 ( uint32_t dat, uint8_t *ptr );
uint8_t GetChkSum ( uint8_t *buff, uint16_t len );
#endif




