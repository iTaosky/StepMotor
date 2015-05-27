#include "includes.h"


void vAM2305Config(void)
{
    /*AM2305      �ܽ�����
	*SDA PD0
    */
	prvAM2305GPIOInit();
	
	prvAM2305NVICInit();
	
	prvAM2305EXTIInit();
	
	prvAM2305TIMInit();

}
static void prvAM2305GPIOInit(void)
{
	/*GPIO��ʼ���ṹ��*/
//	GPIO_InitTypeDef GPIO_InitStructure; 

	 /*����ʱ��GPIOD, TIM3*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		
	 /*GPIO����*/	
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
}
static void prvAM2305NVICInit(void)
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
	
	 /*ʹ��EXTI0_IRQn 12��ռ��ʽ���ȼ�*/
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = AM2305_EXTI_PRIO;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
}
static void prvAM2305EXTIInit(void)
{
	/*EXTI��ʼ���ṹ��*/
	EXTI_InitTypeDef EXTI_InitStructure;

	/*�����ⲿ�ж�ͨ��0�������ش����ж�*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}   
static void prvAM2305TIMInit(void)
{

	/*TIM_TimeBse��ʼ���ṹ��*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_DeInit(TIM3);                              //TIM3��λ

	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;/*71 + 1 ��Ƶ 72 000 000 / 72 = 1 000 000*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM3, DISABLE);
}

