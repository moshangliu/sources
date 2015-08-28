#ifndef __HWTST_H__
#define __HWTST_H__


#define TST_CLOSE    0
#define TST_485_1    1
#define TST_485_2    2
#define TST_CAN_1    3
#define TST_CAN_2    4

#define CAN1_TST_ID  0x155
#define CAN2_TST_ID  0x2aa

typedef struct
{
    uint8_t flg;         // ����Ӳ�����Ա�ʾ
    uint8_t sts;         // ���Թ���״̬��

    uint8_t r485;        // 485���Խ��
    uint8_t rcan;        // 485���Խ��
    uint8_t rin;         // ������Խ��
    uint16_t rtmp[2];    // �¶Ȳ��Խ��
    uint16_t rcurr;      // �������Խ��
    uint8_t  fuse;       // ��˿���Խ��
} HWTST;

extern void hw_tst_init ( void );
extern void hw_tst_main ( void );

#endif

