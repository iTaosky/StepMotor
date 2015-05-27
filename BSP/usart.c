#include "includes.h"

/*
* ��������USART_Config
* ����  ������USART
* ����  ����
* ���  ����
*/ 
void USART_Config()
{
  
    USART_RCC_Config(); 
    USART_GPIO_Config();
    USART_MODE_Config();
    USART_NVIC_Config();
}

/*
* ��������USART_NVIC_Config
* ����  ������USART�ж�
* ����  ����
* ���  ����
*/ 
void USART_NVIC_Config()
{
    /*����NVIC��ʼ���ṹ�� NVIC_InitStructure*/
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /*ѡ��NVIC���ȼ�����4( 0~15��ռ���ȼ�)*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    /*ʹ�ܴ���1�����жϣ���ռ���ȼ� 3 ��Ӧ���ȼ� 0*/
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_RX_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
* ��������USART_RCC_Config
* ����  ������USART�ܽ�ʱ��
* ����  ����
* ���  ����
*/
void USART_RCC_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC
                       | RCC_APB2Periph_AFIO, ENABLE); 
	
	#if uDEBUG > 0u
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	#endif
}

/*
* ��������USART_GPIO_Config
* ����  ������USART�ܽ�״̬
* ����  ����
* ���  ����
*/
void USART_GPIO_Config()
{
    /*����GPIO��ʼ���ṹ�� GPIO_InitStructure*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //******************************************************************************
    //����1��ʹ�ùܽ�������붨��
    //******************************************************************************
    //����UART1 TX (PA.09)��Ϊ�����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         //IO�ڵĵھŽ�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //IO�ڸ����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);            //��ʼ������1���IO��
    
    // ���� USART1 Rx (PA.10)Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;           //IO�ڵĵ�ʮ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//IO����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ������1����IO��

	#if uDEBUG > 0u
	//******************************************************************************
    //����2��ʹ�ùܽ�������붨��
    //******************************************************************************
    //����UART2 TX (PA.2)��Ϊ�����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //IO�ڸ����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);            //��ʼ������1���IO��
    
    // ���� USART2 Rx (PA.3)Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;           //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//IO����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ������2����IO��
    
    
    //******************************************************************************
    //����4��ʹ�ùܽ�������붨��
    //******************************************************************************
    //����UART4 TX (PC.10)��Ϊ�����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;         //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //IO�ڸ����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //��ʼ������4���IO��
    
    // ���� USART4 Rx (PC.11)Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;           //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//IO����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);               //��ʼ������4����IO��
    #endif
    
    // ���� 485 EN (PA.8)Ϊ�������
    //	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
* ��������USART_MODE_Config
* ����  ������USARTģʽ�������ʡ����ݸ�����ֹͣ������ȣ�
* ����  ����
* ���  ����
*/
void USART_MODE_Config()
{
    USART_InitTypeDef USART_InitStructure;
    
    USART_InitStructure.USART_BaudRate = 115200;                                      //�趨��������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�趨��������λ��
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //�趨ֹͣλ����
    USART_InitStructure.USART_Parity = USART_Parity_No ;                            //��У��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //ʹ�ý��պͷ��͹���
    USART_Init(USART1, &USART_InitStructure);                                       //��ʼ������1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                  //ʹ�ܴ���1�����ж�
    USART_Cmd(USART1, ENABLE);                                                      //ʹ�ܴ���1
    //  RS485_R;                                                                        //ʹ�ܽ���

	#if uDEBUG > 0u
		USART_Init(USART2, &USART_InitStructure); 
		USART_Cmd(USART2, ENABLE);
        USART_InitStructure.USART_BaudRate = 9600; 
    	USART_Init(UART4, &USART_InitStructure); 
		USART_Cmd(UART4, ENABLE);
	#endif
}
/*
* ��������FrameBufWrite
* ����  ��������д��������ͨ�Žӿڽ����жϷ�����ã�
* ����  ����
* ���  ����
*/
void FrameBufWrite()
{
    if(FrameBufWptr == (FrameBufRptr - 1))     /*��ֹ��Ȧ*/
    {
      return;
    }
    FrameBuf[FrameBufWptr] = USART_ReceiveData(USART1);
    FrameBufWptr++;
    FrameBufWptr %= FRAMEBUFMAX;
}

/*
* ��������FrameBufRead
* ����  ��������������
* ����  ��data,����Ŷ������ݵ��ڴ�ռ�ָ��
* ���  ��0��������  1��������
*/
u8 FrameBufRead(u8 *data)
{
    if(FrameBufRptr == FrameBufWptr)
    {
      return 0;
    }
    *data = FrameBuf[FrameBufRptr];
    FrameBufRptr++;
    FrameBufRptr %= FRAMEBUFMAX;
    return 1;
}


/*
* ��������UART1_Sendbyte
* ����  �����ֽڷ���
* ����  ��data������������
* ���  ����
*/
void UART1_Sendbyte(INT8U data)
{
    //  RS485_T;
    USART_SendData(USART1, (u8)data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

    
//    OSTimeDly(100); //��֤�����ȶ���
    
    //  RS485_R;
}

/*
* ��������UART1_Sendarray
* ����  ����������
* ����  ��ָ�������׵�ַָ�룬���鳤��
* ���  ����
*/
void UART1_Sendarray(INT8U *buf, INT8U buflen)
{
    u8 i;
    for(i = 0; i < buflen; i++)
    {
      UART1_Sendbyte(buf[i]);
    }
}

/*
* ��������UART4_Sendbyte
* ����  �����ֽڷ���
* ����  ��data������������
* ���  ����
*/
void UART4_Sendbyte(INT8U data)
{

//    USART_SendData(UART4, data);
//    OSTimeDlyHMSM(0, 0, 0, 1);
    
    USART_SendData(UART4, (u8)data);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);

}
void UART4_Sendarray(INT8U *buf, INT8U buflen)
{
    u8 i;
    for(i = 0; i < buflen; i++)
    {
      UART4_Sendbyte(buf[i]);
    }
}


