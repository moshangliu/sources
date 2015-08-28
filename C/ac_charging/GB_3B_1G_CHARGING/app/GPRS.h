#ifndef __GPRS_H__
#define __GPRS_H__

#define heartbeat_cycle 60      //�������ڣ���λ��, 1����
#define Info_cycle 35           //�����ϴ����ڣ���λ�룬35�� 
#define Communication_timeout 600      //ͨ�ų�ʱ����λ�룬10����
//������
#define Heartbeat_Function_code 0x01            //����
#define Account_Verification_Function_code 0x02 //�����֤---�ֶ������û���������
#define QR_code_Verification_Function_code 0x03 //�����֤---ɨ��ά��
#define Start_Function_code 0x04                //�����������
#define Stop_Function_code 0x05                 //���ֹͣ����
#define AC_Info_Function_code 0x06              //�����ϴ�---����
#define DC_Info_Function_code 0x07              //�����ϴ�---ֱ��
#define APP_Stop_Function_code 0x08             //APP������ֹͣ
#define Card_Verification_Function_code 0x09    //�����֤---���û�
#define calibration_time_Function_code 0x0a     //Уʱ
//�����򳤶�
#define Len_Heartbeat 0x02
#define Len_Account_Verification 0x12
#define Len_QR_code_Verification 0
#define Len_Start 0x10
#define Len_Stop 0x08
#define Len_AC_Info 0x0c
#define Len_DC_Info 0x0d
#define Len_APP_Stop 0
#define Len_Card_Verification 0x16
#define Len_calibration_time 0

typedef struct
{
    uint8_t tel_number[12];         //�ֻ���,��һλ0������11λΪ��ʵ�ֻ��ţ�ASCII
    uint8_t password[6];        //���룬ASCII
    uint8_t card_number[16];    //���ţ�ASCII
    uint8_t card_status;        //��״̬
    uint32_t Balance;           //���
    uint32_t pay;               //���ѽ��
} user_info;

extern void GPRS_main( void );


#endif

