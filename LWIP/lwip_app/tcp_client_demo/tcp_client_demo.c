
#include "lwip/sockets.h"
#include "netif/etharp.h"
#include "lwip/arch.h"
#include "ethernetif.h" 
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include "tcp_client_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include "includes.h"

//int errno;
   
struct netconn *tcp_clientconn;					//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 *tcp_client_sendbuf="Explorer STM32F103 NETCONN TCP Client send data\r\n";	//TCP�ͻ��˷������ݻ�����
u8 tcp_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ

void *pff[2] = 
{
  tcp_client_thread_netconn,
  tcp_client_thread_socket  
};


//TCP�ͻ�������
#define TCPCLIENT_PRIO		6
//�����ջ��С
#define TCPCLIENT_STK_SIZE	300
//�����ջ
OS_STK TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE];
/******************socket  start****************************/
static void tcp_client_thread_socket (void *arg)
{

    s8                   ClientSocket;
	err_t                err;
    int                  Sockerr;
    static u16_t 		 server_port;
	static ip_addr_t     server_ipaddr;
    struct sockaddr_in  nameaddr;          /*��ַ�ṹ�嶨��*/
    tcp_client_flag = 0x80;
    
	LWIP_UNUSED_ARG(arg); /*��ֹ����������*/
    server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	
    memset(&nameaddr, 0, sizeof(nameaddr));
	
    nameaddr.sin_family  = AF_INET;                /*AF_INET ʹ��TCP*/
    nameaddr.sin_port = htons(server_port);               /*�˿ں� ת��Ϊ�����ֽ���*/
	nameaddr.sin_addr.s_addr = server_ipaddr.addr;  /*IP��ַ��ֵ 32λ*/
    while(1)
    {
        ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(ClientSocket >= 0) /*socket����ɹ�*/
		{
            err = connect(ClientSocket, (const struct sockaddr *)&nameaddr, sizeof(nameaddr));
			if(err == ERR_OK)
			{
				printf("�����Ϸ�����%d.%d.%d.%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
                while(1)
				{
                    if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
                    {
                        tcp_client_flag = 0;
                        err = send(ClientSocket, (const void *)tcp_client_sendbuf, strlen((char*)tcp_client_sendbuf), 0);
                        if(err < 0)
                            printf("��������ʧ�� err = %d", err);
                        else 
                            printf("���ͳɹ� ���ݳ��� %d", err);
                        err = 0;
                    }
                    err = recv(ClientSocket, tcp_client_recvbuf, TCP_CLIENT_RX_BUFSIZE, MSG_DONTWAIT);
                    Sockerr = errno;
                    if(err > 0)
                    {
                       			
                        printf("%s\r\n",tcp_client_recvbuf); 
                        send(ClientSocket, (const void *)tcp_client_recvbuf, err, 0);
                        OSTimeDlyHMSM(0, 0, 0, 60);
 
                        //memset(tcp_client_recvbuf, 0, TCP_CLIENT_RX_BUFSIZE);
                    }
                    else if((err  ==  -1) && (Sockerr == EWOULDBLOCK))
                    {
                        //close(ClientSocket);
                        //break;
                        OSTimeDlyHMSM(0, 0, 0, 60);
                    }
                    else 
                    {
                        close(ClientSocket);
                        break;                        
                    }
                     
			    }
			}
			else
			{
            	printf("����ʧ�� err = %d", err);
			}
		}
		else
		{
			close(ClientSocket);
			printf("Scoket ����ʧ��err = %d", err);
		}
        OSTimeDlyHMSM(0, 0, 0, 60); 
    }
}


/******************socket  end****************************/

/******************netconn start****************************/
//tcp�ͻ��������� netconn��ʽ
static void tcp_client_thread_netconn(void *arg)
{
	OS_CPU_SR cpu_sr;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;

	LWIP_UNUSED_ARG(arg);
	server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	
	while (1) 
	{
		tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//���ӷ�����
		if(err != ERR_OK)  netconn_delete(tcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					err = netconn_write(tcp_clientconn ,tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf),NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					tcp_client_flag &= ~LWIP_SEND_DATA;
				}
					
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					OS_ENTER_CRITICAL(); //���ж�
					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					OS_EXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					printf("%s\r\n",tcp_client_recvbuf);
                    err = netconn_write(tcp_clientconn ,tcp_client_recvbuf,strlen((char*)tcp_client_recvbuf),NETCONN_COPY); //����tcp_server_sentbuf�е�����
                    netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
			}
		}
	}
}
/********************netconn end*************************/
//����TCP�ͻ����߳�
//����ֵ:0 TCP�ͻ��˴����ɹ�
//		���� TCP�ͻ��˴���ʧ��
INT8U tcp_client_init(u8 flag)
{
	INT8U res;
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();	//���ж�
	res = OSTaskCreate(pff[flag],(void*)0,(OS_STK*)&TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE-1],TCPCLIENT_PRIO); //����TCP�ͻ����߳�
	OS_EXIT_CRITICAL();		//���ж�
	
	return res;
}

