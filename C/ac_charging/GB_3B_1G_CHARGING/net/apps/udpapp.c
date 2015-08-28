#include "stm32f10x.h"
#include  "uip.h"  
#include  "enc28j60.h"
#include "uipopt.h"
#include "uip_arch.h"
#include <string.h>	 

#define LED1_ON()	GPIO_SetBits(GPIOB,  GPIO_Pin_5);
#define LED2_ON()	GPIO_SetBits(GPIOD,  GPIO_Pin_6);
#define LED3_ON()	GPIO_SetBits(GPIOD,  GPIO_Pin_3);

#define LED1_OFF()	GPIO_ResetBits(GPIOB,  GPIO_Pin_5);
#define LED2_OFF()	GPIO_ResetBits(GPIOD,  GPIO_Pin_6);
#define LED3_OFF()	GPIO_ResetBits(GPIOD,  GPIO_Pin_3);

extern unsigned short LPORT;

extern void tcp_demo_appcall(void);
void myudp_appcall(void);


/*******************************************************************************
*	������: void myudp_send(char *str,short n) 
*	��  ��: 
*	��  ��: ��
*	��  ��: UDP ���ݰ�����
**************************************************************************/
void myudp_send(char *str,short n) 
{ 

   char   *nptr;   
   nptr = (char *)uip_appdata;       
   memcpy(nptr, str, n); 
   uip_udp_send(n);   				//����n������ 
} 

/*******************************************************************************
*	������: void UDP_newdata(void) 
*	��  ��: 
*	��  ��: ��
*	��  ��: UDP ���ݰ�����
**************************************************************************/
void UDP_newdata(void) 
{ 
    char   *nptr; 
    short len; 
	len = uip_datalen();									//��ȡ���ݳ��� 
    nptr = (char *)uip_appdata; 							//ȡ��������ʼָ�� 
	if(len<4)myudp_send("Please check the command!\n",26); 
    else if(strncmp(nptr,"getname",7)==0) myudp_send("���Ƿܶ�STM32������\n",19); 
	else if(strncmp(nptr,"ledon 1",7)==0){
		LED1_ON();
		myudp_send("LED1 ��\n",8);
	} 
	else if(strncmp(nptr,"ledon 2",7)==0){
		LED2_ON();
		myudp_send("LED2 ��\n",8);
	}
	else if(strncmp(nptr,"ledon 3",7)==0){
		LED3_ON();
		myudp_send("LED3 ��\n",8);
	}
	else if(strncmp(nptr,"ledoff 1",8)==0){
		LED1_OFF();
		myudp_send("LED1 ��\n",8);
	} 
	else if(strncmp(nptr,"ledoff 2",8)==0){
		LED2_OFF();
		myudp_send("LED2 ��\n",8);
	}
	else if(strncmp(nptr,"ledoff 3",8)==0){
		LED3_OFF();
		myudp_send("LED3 ��\n",8);
	}

    else myudp_send("Unkown command!\n",16); 
} 
/*******************************************************************************
*	������: void myudp_appcall(void) ) 
*	��  ��: 
*	��  ��: ��
*	��  ��: UDP������
**************************************************************************/
void myudp_appcall(void) 
{ 
   if(uip_newdata()) 						
   { 
       UDP_newdata();      
   } 
} 

/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/


