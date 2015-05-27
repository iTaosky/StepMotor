#ifndef __TCP_CLIENT_DEMO_H
#define __TCP_CLIENT_DEMO_H

#include "includes.h"
   
 
 
#define TCP_CLIENT_RX_BUFSIZE	2048	//���ջ���������
#define REMOTE_PORT				8087	    //����Զ�������˿�
#define LWIP_SEND_DATA			0X80    //���������ݷ���

#define xNETCONN 0
#define xSOCKET 1


extern u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
extern u8 tcp_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ

u8 tcp_client_init(u8 flag);  //tcp�ͻ��˳�ʼ��(����tcp�ͻ����߳�) 
                              //������flag xNETCONN(0) NETCONN��ʽ xSOCKET(1) socket��ʽ
                              
                              
static void tcp_client_thread_socket (void *arg);
static void tcp_client_thread_netconn(void *arg);
#endif

