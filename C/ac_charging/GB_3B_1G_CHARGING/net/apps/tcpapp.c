#include "appcall.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "drv_inc.h"
#include "tcpip.h"
static void aborted(void);
static void timedout(void);
static void closed(void);
static void connected(void);
static void newdata(void);
static void acked(void);
static void senddata(void);

static uint8_t test_data[2048];   /* 1K�������� */


/*******************************************************************************
*	������: tcp_demo_appcall
*	��  ��: ��
*	��  ��: ��
*	����˵��������һ���ص���������h�ļ��У����Ƕ��� UIP_APPCALL�ĺ���� tcp_demo_appcall
*		��uip�¼�����ʱ��UIP_APPCALL �����ᱻ���á�
*		���� : ��һ��TCP���ӱ�����ʱ�����µ����ݵ�������Ѿ���Ӧ��������Ҫ�ط����¼�
*
***************************************************************************/
void tcp_appcall(void)
{
	if (uip_aborted())//������ߣ��������ӷ�����
	{
		aborted();
	}

	if (uip_timedout())//�����ʱ���ط�
	{
		timedout();
	}

	if (uip_closed())
	{
		closed();
	}

	if (uip_connected())//�����ӣ���Ӧ
	{
		connected();
	}

	if (uip_acked())//Ӧ��
	{
		acked();
	}

	/* ���յ�һ���µ�TCP���ݰ���׼����Ҫ�������� */
	if (uip_newdata())//��������
	{
		newdata();
	}

	/* ����Ҫ�ط��������ݵ�����ݰ��ʹ���ӽ���ʱ��֪ͨuip�������� */
	if (uip_rexmit() ||	uip_newdata() || uip_acked() ||	uip_connected() || uip_poll())
	{
		senddata();
	}
}

/*******************************************************************************
*	������: aborted
*	��  ��: ��
*	��  ��: ��
*	����˵������TCP�����쳣��ֹʱ�����ô˺�����
***************************************************************************/
static void aborted(void)
{
	ConnectedFlag = 0 ;//��ʾ��������
}

static void timedout(void)
{
	;
}

static void closed(void)
{
	;
}

/*******************************************************************************
*	������: connected
*	��  ��: ��
*	��  ��: ��
*	����˵������TCP���ӽ���ʱ�����ô˺�����
***************************************************************************/
static void connected(void)
{  
	/*
		uip_conn�ṹ����һ��"appstate"�ֶ�ָ��Ӧ�ó����Զ���Ľṹ�塣
		����һ��sָ�룬��Ϊ�˱���ʹ�á�

		����Ҫ�ٵ���Ϊÿ��uip_conn�����ڴ棬����Ѿ���uip�з�����ˡ�
		��uip.c �� ����ش������£�
			struct uip_conn *uip_conn;
			struct uip_conn uip_conns[UIP_CONNS]; //UIP_CONNSȱʡ=10
		������1�����ӵ����飬֧��ͬʱ�����������ӡ�
		uip_conn��һ��ȫ�ֵ�ָ�룬ָ��ǰ��tcp��udp���ӡ�
	*/
        ConnectedFlag = 1 ;//��ʾ���ӳɹ�
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;

	memset(test_data, 0x55, 2048);

	s->state = STATE_CMD;						        //ָ��״̬
	s->textlen = 0;

	s->textptr = "Connect STM32-FD Board Success!";
	s->textlen = strlen((char *)s->textptr);
}

/*******************************************************************************
*	������: TCPCmd
*	��  ��: ��
*	��  ��: ��
*	����˵��������PC������������͵������������Ӧ�Ĵ���
***************************************************************************/
void TCP_Cmd(struct tcp_demo_appstate *s)
{
	uint8_t led;
	
;
}

/*******************************************************************************
*	������: newdata
*	��  ��: ��
*	��  ��: ��
*	����˵�������յ��µ�TCP��ʱ�����ô˺�����׼�����ݣ�������ʱ�����͡�
***************************************************************************/
static void newdata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	
	if (s->state == STATE_CMD)
	{
		TCP_Cmd(s);
	}
	else if (s->state == STATE_TX_TEST)	/* �ϴ�����״̬ */
	{
		/* �ڷ��Ͳ���״̬������յ�PC�����͵��������ݣ����˳�����״̬ */
		if ((uip_len == 1) && (((uint8_t *)uip_appdata)[0] == 'A'))
		{
			;/* �������� */
		}
		else
		{
			/* �˵�����״̬ */
	   		s->state = STATE_CMD;
			s->textlen = 0;
		}
	}
	else if (s->state == STATE_RX_TEST)	/* �´�����״̬ */
	{				
		if ((uip_len == 4) && (memcmp("stop", uip_appdata, 4) == 0))
		{
			/* �˵�����״̬ */
	   		s->state = STATE_CMD;
			s->textlen = 0;
		}
		else
		{ 			
			s->textptr = uip_appdata;   	/* ��ͻ��˷����յ������� */
			s->textlen = uip_len;
		}
	}
}

/*******************************************************************************
*	������: acked
*	��  ��: ��
*	��  ��: ��
*	����˵���������͵�TCP���ɹ��ʹ�ʱ�����ô˺�����
***************************************************************************/
static void acked(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;

	switch(s->state)
	{
		case STATE_CMD:		 /* ������״̬ */
			s->textlen = 0;

			/* 
				ֻ������״̬��ӡ������Ϣ 
				���ⷢ�Ͳ���ʱ��Ӱ��ͨ���ٶ�		
			*/
			break;

		case STATE_TX_TEST:
			s->textptr = test_data;	/* �������� */
			s->textlen = 1400;
			break;

		case STATE_RX_TEST:
			s->textlen = 0;
			break;
	}
}

/*******************************************************************************
*	������: senddata
*	��  ��: ��
*	��  ��: ��
*	����˵��������tcp���ݡ�
***************************************************************************/
static void senddata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;

	if (s->textlen > 0)
	{
		/*
			��������������緢��TCP���ݰ�,
				s->textptr : ���͵����ݰ�������ָ��
				s->textlen �����ݰ��Ĵ�С����λ�ֽڣ�
		*/
		uip_send(s->textptr, s->textlen);
	}
}


