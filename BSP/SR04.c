#include "includes.h"


void vSR04Config(void)
{
    /*SR04      �ܽ�����
	*TRIG(PE8)  �������
	*ECHO(PE7)  ��������  �ⲿ�ж�
    */
	prvSR04GPIOInit();
	
	prvSR04NVICInit();
	
	prvSR04EXTIInit();
	
	prvSR04TIMInit();

}
static void prvSR04GPIOInit(void)
{
	/*GPIO��ʼ���ṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure; 

	 /*����ʱ��GPIOC, GPIOE, TIM8,AFIO*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOD  | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
	 /*GPIO����*/	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource7);


}
static void prvSR04NVICInit(void)
{
	 /*NVIC��ʼ���ṹ��*/
	NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef VECT_TAB_RAM 	   
	 /* Set the Vector Table base location at 0x20000000 */ 	  
	 NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else /* VECT_TAB_FLASH */		  
	 /* Set the Vector Table base location at 0x08000000 */ 	  
	 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	
	#endif	 
	
	 /* ѡ��NVIC���ȷ���4 ����4λ������ռʽ���ȼ�  */
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
	
	 /*ʹ��EXTI9_5_IRQn 12��ռ��ʽ���ȼ������ڳ�������Ҫʹ���ź���*/
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = SR04_EXTI_PRIO;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);


}
static void prvSR04EXTIInit(void)
{
	/*EXTI��ʼ���ṹ��*/
	EXTI_InitTypeDef EXTI_InitStructure;

	/*�����ⲿ�ж�ͨ��7�������ش����ж�*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}   
static void prvSR04TIMInit(void)
{

	/*TIM_TimeBse��ʼ���ṹ��*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_DeInit(TIM2);                              //TIM2��λ

	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;/*71 + 1 ��Ƶ 72 000 000 / 72 = 1 000 000*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM2, DISABLE);
}

