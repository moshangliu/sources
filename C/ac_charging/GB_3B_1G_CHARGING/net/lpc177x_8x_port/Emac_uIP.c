/**********************************************************
���ռ�������Դ�������޹�˾
File name:  Emac_uIP.c	              // �ļ���
Author:     ����                    // ����
Version:    v1.00                   // �汾��Ϣ
Date:       2013-9-6               // �������
Description:  	              // ʵ��UIP��ʼ����ͨ�Ź���
Others:		              // ��
Function List:	              //
**********************************************************/
#include <stdio.h>
#include <string.h>
#include "drv_inc.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip-conf.h"
#include "uipopt.h"
#include "uip_arp.h"
#include "uip.h"
#include "emac.h"
#include "tcpip.h"


unsigned char net_timeover10=0;
/************************** PRIVATE DEFINITIONS ***********************/
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
uint16_t net_time05=0;
uint16_t net_time10=0;
uint8_t ConnectedFlag;

void NetTimerDeal(void)
{
    net_time05++;
    net_time10++;
}
/*******************************************************************************
*	NetLowLevelInit
*	��  ��:
*	��  ��:
*	����˵������ʼ������Ӳ����UIPЭ��ջ�����ñ���IP��ַ
************************************************************/
void NetLowLevelInit(void)
{
    uip_ipaddr_t ipaddr;
    tapdev_init();                     		 //Ӳ����ʼ��
    uip_arp_init();
    uip_init();								 //UIPЭ��ջ��ʼ��
    uip_ethaddr.addr[0] = MyMAC[0];
    uip_ethaddr.addr[1] = MyMAC[1];
    uip_ethaddr.addr[2] = MyMAC[2];
    uip_ethaddr.addr[3] = MyMAC[3];
    uip_ethaddr.addr[4] = MyMAC[4];
    uip_ethaddr.addr[5] = MyMAC[5];
    uip_setethaddr(uip_ethaddr);    
    
    uip_ipaddr(ipaddr, MyIP[0]&0xff,MyIP[0]>>8,MyIP[1]&0xff,MyIP[1]>>8);		 //����IP��ַ
    //uip_ipaddr(ipaddr, 192,168,1,15);
    uip_sethostaddr(ipaddr);
    
    uip_ipaddr(ipaddr, GatewayIP[0]&0xff,GatewayIP[0]>>8,GatewayIP[1]&0xff,GatewayIP[1]>>8);//����Ĭ��·����IP��ַ
    //uip_ipaddr(ipaddr, 192,168,1,1);
    uip_setdraddr(ipaddr);
    
    uip_ipaddr(ipaddr, SubnetMask[0]&0xff,SubnetMask[0]>>8,SubnetMask[1]&0xff,SubnetMask[1]>>8);		 //������������
    //uip_ipaddr(ipaddr, 255,255,255,0);
    uip_setnetmask(ipaddr);
    
    /* ����һ��TCP�����˿ں�http�����˿ڣ��˿ں�Ϊ1200��80 */
    uip_listen(HTONS(TCPLocalPort));//
}

void ConnToServer(void)
{
    struct uip_conn *u;
    uip_ipaddr_t ipaddr;
    
    uip_ipaddr(ipaddr, 192,168,100,100); // Ŀ�ĵ�ַ  
    u=uip_connect((uip_ipaddr_t*)ipaddr , HTONS(8000) ); 
    if(u==NULL)
    {
        //����Ҫ��������                
        return ;
    }
    uip_periodic_conn(u);
    if(uip_len== 0)
    {
        uip_periodic_conn(u);//������uip_connectʱconn->timer = 1,"
        //"���Ա������uip_periodic_conn����uip�Ż�׼������"
    } 
    if(uip_len > 0)
    {
        uip_arp_out();
        tapdev_send(uip_buf,uip_len);
    }      
}
/****************************************************************************
* ��    �ƣ�void EthPoll(void)
* ��    �ܣ���ʱ��ѯTCP�����շ�״̬	ARP����£� ����Ӧ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void EthPoll(void){
    unsigned char i=0;
    if(net_time05>=500)			/* 0.5�붨ʱ����ʱ */
    {
        net_time05=0;
//        net_timeover05=0;			/* ��λ0.5�붨ʱ�� */
        
        /* ��������ÿ��TCP����, UIP_CONNSȱʡ��10�� */
        for(i = 0; i < UIP_CONNS; i++)
        {
            uip_periodic(i);		/* ����TCPͨ���¼� */
            /*
            ������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
            ��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
            */
            if(uip_len > 0)
            {
                uip_arp_out();		//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
                tapdev_send(uip_buf,uip_len);		//�������ݵ���̫�����豸��������
            }
        }
        
#if UIP_UDP
        /* ��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10�� */
        for(i = 0; i < UIP_UDP_CONNS; i++)
        {
            uip_udp_periodic(i);	/*����UDPͨ���¼� */
            /* �������ĺ������õ�������Ӧ�ñ����ͳ�ȥ��ȫ�ֱ���uip_len�趨ֵ> 0 */
            if(uip_len > 0)
            {
                uip_arp_out();		//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
                tapdev_send(uip_buf,uip_len);		//�������ݵ���̫�����豸��������
            }
        }
#endif /* UIP_UDP */
        
        /* ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����*/
        if (net_time10>=10000)
        {
            net_time10=0;
            //net_timeover10=0;		/* ��λ10�붨ʱ�� */
            uip_arp_timer();
        }
    }
}
/*******************************************************************************
*	��������UipPro
*	��  ��:
*	��  ��:
*	����˵�����жϴ�����ȡ������ջ���
********************************************************************/
void UipPro(void)
{
      uip_len = tapdev_read(uip_buf);	//�������豸��ȡһ��IP��,�������ݳ���
      if(uip_len > 0)			    //�յ�����
      {
          /* ����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) */
          if(BUF->type == htons(UIP_ETHTYPE_IP))   //��IP����
          {
              uip_arp_ipin();		   //ȥ����̫��ͷ�ṹ������ARP��
              uip_input();		   //IP������
              /*
              ������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
              ��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
              */
              if (uip_len > 0)		//�д����Ӧ����
              {
                  uip_arp_out();		//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
                  tapdev_send(uip_buf,uip_len);		//�������ݵ���̫�����豸��������
              }
          }
          /* ����arp���� */
          else if (BUF->type == htons(UIP_ETHTYPE_ARP))	//��ARP�����
          {
              uip_arp_arpin();		//������ARP��Ӧ������ARP����������󣬹����Ӧ���ݰ�
              /*
              ������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
              ��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
              */
              if (uip_len > 0)		//��ARP����Ҫ���ͻ�Ӧ
              {
                  tapdev_send(uip_buf,uip_len);		//��ARP��Ӧ����̫����
              }
          }
      }
      else
      {
          EthPoll(); //��ʱ��ѯTCP��UDP�����շ�״̬	ARP����£� ����Ӧ
      }
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*************************************************************
Function: void NetPro(void)
Description: ����UIP�¼�
Calls:
Called By:   
Input:       
Output:      
Return:      
Others:      
*************************************************************/
void NetPro(void)
{
    /* ����uip�¼���������뵽�û������ѭ������ */
/*    if (ConnectedFlag==0)
    {
        ConnToServer();
        ConnectedFlag = 2 ;
    }*/
    UipPro();	//��ȡ������ջ���
}
