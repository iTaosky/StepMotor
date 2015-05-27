#include "includes.h"

/*ȫ�ֱ���*/
vu8 fSR04Distance = 0;  /*SR04��þ����ű���*/ 
/*END*/

/*�ֲ�����*/
vu8 EXTI_LINE7_FLAG = 0; /*SR04������־λ��0��������ϻ��޲�������2��������ʼ�������أ�1���������̣��½���*/
u16 *pp;                /*���������м�ֵ*/        
OS_EVENT *SR04CommMbox; /*�������ݴ�������*/      
/*END*/

void vTaskSR04(void *p_arg)
{ 
	u8 err;             /*����ȴ�����ָʾ*/ 
    u16 Pulse, pa, pb;  /*�������ݼ����м�ֵ��ָ�븳ֵר��*/
	void *msg;          /*��������ָ�붨��*/
    
    pp = &pa;           /*ָ���ʼ��*/
    msg = &pb;          /*ָ���ʼ��*/
    
    (void)p_arg;    /*'p_arg' ��û���õ�����ֹ��������ʾ����*/
    SR04CommMbox = OSMboxCreate((void *)0); /*��������*/
	
    
    for(;;)
    {
        /*��������*/
        OSTaskSuspend(TASK_AM2305_PRIO);
        
        GPIO_SetBits(GPIOE, GPIO_Pin_8);       /*����Trig�ܽ�*/
        EXTI_LINE7_FLAG = 2;                   /*������־λ��ֵ*/
        delay_10us();						   /*����10us����*/                        
        GPIO_ResetBits(GPIOE, GPIO_Pin_8);     /*����Trig�ܽţ�����SR04����*/
        /*end*/
        
        /*�ȴ������ź�*/
        msg = OSMboxPend(SR04CommMbox, 5, &err);
        OSTaskResume(TASK_AM2305_PRIO);
        if(err == OS_ERR_NONE)
        {
            Pulse = *((u16*)msg);
            fSR04Distance = ((Pulse) * 340) / 20000; /*���� = (����ƽ��ֵ * ����) / (1000 * 2) mm */
        }        
        OSTimeDlyHMSM(0, 0, 0, 50);                   
    }
}
/*�����ʱ 10us*/
void delay_10us(void)
{
    u8 n;
    for(n = 13; n > 0; n--);
}

void delay_ms(unsigned int i)
{
    u8 n;
    while(i--)
    {
        for(n = 0;n < 100;n++)
            delay_10us();
    }
}

void vtaskSR04Handle(void)
{
	if(EXTI_LINE7_FLAG == 2)
	{
		TIM_SetCounter(TIM2, 0);  /*�������ж� ��ʱ���� �������*/
		TIM_Cmd(TIM2, ENABLE);
		EXTI_LINE7_FLAG --;
	}
	else if(EXTI_LINE7_FLAG == 1)
	{
		*pp = TIM_GetCounter(TIM2);/*��ȡ����*/
		TIM_Cmd(TIM2, DISABLE);    /*�½����ж� ��ʱ���ر� �������*/
        TIM_SetCounter(TIM2, 0);
		EXTI_LINE7_FLAG = 0;       /*��־λ��0*/
		OSMboxPost(SR04CommMbox, (void *)(pp));/*������������*/
	}
}

void EXTI9_5_IRQHandler(void)
{
    OSIntEnter();
	
	if(EXTI_GetITStatus(EXTI_Line7) == SET)
	{
		vtaskSR04Handle();   /*���ô�����*/
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
    
	OSIntExit();
}
