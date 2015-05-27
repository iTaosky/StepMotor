#include "includes.h"

vu8   MotorActionState;//���Ӽ��������������״̬
                       //0:�޶���   1����Ҫ�������״̬   2�����ٹ�����  ������� ������ٻ����  3���������  �������
                       //4:��Ҫ�������״̬               5�����ٹ�����  ������� ֹͣ ����״̬ 0
vu8   MotorDir; //1����ת 2����ת 0:������
vu8   HomeFlag,CWFlag,CCWFlag; // ԭ�㡢������λ״̬ CWFlag Ϊ1ʱ �޷�����������   CCWFlag ͬ��
vu8   ACC_Status_Flag, DEC_Status_Flag, ConstantVelocity_Status_Flag;    //���� ���� ����״̬��־   
u16   PulseNum_Buf[50], PulseNum_Sum_Buf[50];//�����������
u16   MotorCycle_Buf[50]; //���ڱ���  ��λ 10us  �Ӽ��ٹ��� ����

vu32   MotorFreqTarget, MotorCycleTarget;//Ŀ��Ƶ��
vu32   MotorFreqReal, MotorCycleReal;  //ʵʱƵ��  Cycle ��λ 10us
vu32   PulseNum_Global;//����ȫ�ֱ���
vu32   PulseNum_TEMP;
vu32   PulseNum_ACC, PulseNum_DEC, PulseNum_ConstantVel;//�м���� ���������� ָʾ�Ӽ��ٸ��׶����趯��������  �ݶ���Ӽ����м�Ƶ����ͬ
vu32   PulseNum_Exti;

void Motor_Init(u16 TIM4per, u16 TIM5per, u16 TIM5Compare1);

void vPTOConfig(void)
{
    u8 i;
    
    /*���ݺ궨����мӼ����м�������*/
    for(i = 0; i < 50; i++)
    {
        MotorCycle_Buf[i] = 100000 / (ACC_HZ_MIN * (i + 1));
    }
    for(i = 0; i < 50; i++)
    {
        PulseNum_Buf[i] = (ACC_PERIOD *  100)/ MotorCycle_Buf[i];
    }
    PulseNum_Sum_Buf[0] = PulseNum_Buf[0];
    for(i = 1; i < 50; i++)
    {
        PulseNum_Sum_Buf[i] = PulseNum_Sum_Buf[i-1] + PulseNum_Buf[i];
    }
    
    MotorCycleReal = 0;
    PulseNum_Global = 0;
    Motor_Init(100, 100, 50);  
    
    TIM_SetCounter(TIM1, 0);
}

/****************************
**TIM5��ͨ��1ʹ������ģʽ  
**
**
****************************/
//TIM5per:��װֵ
//Compare1:�Ƚϲ���1��Ԥװ��ֵ
void Motor_Init(u16 TIM5per, u16 TIM1per, u16 TIM1Compare1)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
		NVIC_InitTypeDef NVIC_InitStructure;
		/*EXTI��ʼ���ṹ��*/
		EXTI_InitTypeDef EXTI_InitStructure;
    
    /*����ʱ�� �� ��ʱ�� 1 5��PE9  ����ʱ�� ������� ���� ����ʹ�ùܽ�ӳ�� ֱ�� ʹ��PA8 ע����Ĺܽ�ʱ�Ӻ� ��ʼ�����*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,  ENABLE);//ʹ�ܶ�ʱ��1��ʱ��
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,  ENABLE);//ʹ�ܶ�ʱ��5��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);//ʹ�ܸ���IOʱ��
    
    /*�������ź� �ܽ�����*/
		/*����ʱ��GPIOD*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		/*�ⲿ�ж�GPIOD1*/
		GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_1| GPIO_Pin_2 | GPIO_Pin_4;	
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStruct);
	
//		GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_2;	
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
//		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOD, &GPIO_InitStruct);	
	
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource4);
	/*OVER*/
    
    
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE); ////Timer1��ȫӳ��  TIM1_CH1->PE9  TIM2_CH2->PE11  
    
    /*���ö�ʱ��1  TIM1_CH1 PE9 Ϊ ��������*/
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    
    
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//�����������
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    
    /*���÷���IO PE0  Ϊ ����*/
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//�����������
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE,&GPIO_InitStruct);
    
////    /*TIM5 ��ʱ��ʱ�� ����  */
////    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//û��ʱ�ӷָ�
////    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
////    TIM_TimeBaseInitStruct.TIM_Prescaler = 7199;//Ԥ��Ƶֵ,ÿ100us����һ��
////    TIM_TimeBaseInitStruct.TIM_Period = TIM5per;//��װֵ
////    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);  
////    
////    TIM_ITConfig(TIM5, TIM_IT_Update|TIM_IT_Trigger,ENABLE);//ʹ�ܶ�ʱ��5���´����ж�
////    TIM_ARRPreloadConfig(TIM5, ENABLE);//ʹ����װ��
////    TIM_Cmd(TIM5, DISABLE);  //��ʱ��ʹ��TIMx����
    
		
		/*NPN���������ⲿ�ж�ͨ��1�������ش����ж�*/
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		/*NPN���������ⲿ�ж�ͨ��2�������ش����ж�*/
		EXTI_InitStructure.EXTI_Line = EXTI_Line2;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
        
        /*ԭ�㴥���ж�*/
		EXTI_InitStructure.EXTI_Line = EXTI_Line4;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
       
	#ifdef VECT_TAB_RAM 	   
	 /* Set the Vector Table base location at 0x20000000 */ 	  
	 NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else /* VECT_TAB_FLASH */		  
	 /* Set the Vector Table base location at 0x08000000 */ 	  
	 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	
	#endif	 
	
	/* ѡ��NVIC���ȷ���4 ����4λ������ռʽ���ȼ�  */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
////    /*��ʱ��5�����ж�����*/ 
////    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM5�ж�
////    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_TIM_PRIO;  //��ռ���ȼ�
////    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�
////    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
////    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 	
   
   /*ʹ��EXTI1_IRQn ���ȼ�*/
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LS_EXTI_PRIO;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	 
	    /*ʹ��EXTI2_IRQn ���ȼ�*/
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LS_EXTI_PRIO;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	 
     	    /*ʹ��ԭ�� ���ȼ�*/
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LS_EXTI_PRIO;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	 
    /*TIM1 ���������ʱ������*/
    //TIM1���� Ԥ��Ƶֵ
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//û��ʱ�ӷָ�
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
    TIM_TimeBaseInitStruct.TIM_Prescaler = 719;//Ԥ��Ƶֵ,ÿ10us����һ��
    TIM_TimeBaseInitStruct.TIM_Period = TIM1per;//��װֵ
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
    
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//�����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1���ʹ��
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//��Ч��ƽΪ��
    TIM_OCInitStruct.TIM_Pulse = TIM1Compare1;//  �Ƚϲ���5��Ԥװ��ֵ
    TIM_OC2Init(TIM1,&TIM_OCInitStruct);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);   //�߼���ʱ����Ҫʹ�ô˺���
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);//ʹ�ܶ�ʱ��1��ͨ��1Ԥװ�ؼĴ���
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);//ʹ�ܶ�ʱ��1���´����ж�

    TIM_Cmd(TIM1,DISABLE);//�Ȳ�ʹ����TIM1
    
    /*��ʱ��1�����ж�����*/ 
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM5�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_TIM1_PRIO;  //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 	
     
  /*TIM1 END*/
}

/*******************************************************************/
/***********************λ��ģʽ************************************/
/*******************************************************************/
/*******************************************************************/
/*  Freq:       Ƶ��
*   Pulse_Num:  ���� ������  λ��
*
*
*/
void PTO_HZ_NUM(unsigned short Freq, unsigned int Pulse_Num)
{
    u8 i;
    s32 PulseNum_Rel;        //���λ�ñ���
    MotorFreqTarget = Freq;  
    MotorCycleTarget = 100000 / Freq;  //��λ  10us   ����Ϊ���������ʱ�� ��С������λ
    
    ACC_Status_Flag = (u8)(MotorFreqTarget / ACC_HZ_MIN);  //������ٽ׶�  �� �Ե�ǰ���ٶ� ��Ҫ���м��μ���
    DEC_Status_Flag = ACC_Status_Flag;
    
    PulseNum_Rel = Pulse_Num - PulseNum_Global;//����λ�������� ������
    
    /*1:���������ж�*/
    if(PulseNum_Rel == 0)
    {
        MotorDir = 0;
        GPIO_ResetBits(GPIOE, GPIO_Pin_0);
    }
    else if(PulseNum_Rel > 0) //�趨���������� ��ǰ����λ��
    {
        MotorDir = 1;
        GPIO_ResetBits(GPIOE, GPIO_Pin_0);//��ת
    }
    else if(PulseNum_Rel < 0) //�趨������С�� ��ǰ����λ��
    {
        PulseNum_Rel = -PulseNum_Rel;
        MotorDir = 2;
        GPIO_SetBits(GPIOE, GPIO_Pin_0);//��ת
    }
    /*end 1*/
    
    /*2:�趨����Ƶ�ʴ�����ʼƵ��*/
    if((ACC_Status_Flag != 0) && ( PulseNum_Rel != 0))
    {   /*λ������������ 2�� ���ٵ�ָ��Ƶ�ʵ�������*/
        if(PulseNum_Rel > (2*PulseNum_Sum_Buf[ACC_Status_Flag - 1] - 1))  //�ݶ� ���ٺͼ��ٶ�һ�� �����ٺͼ��� ���������������ͬ��
        {                                                           //���ָ������������2����ֵ �������ȫ���ٹ���
            PTO_HZ_NUM_Start(ACC_Status_Flag, PulseNum_Rel);
        }
        /*λ��������С��2�� */
        else if(PulseNum_Rel < 2*PulseNum_Sum_Buf[ACC_Status_Flag - 1])
        {
            for(i = ACC_Status_Flag; i > 0; i--)
            {
                if(PulseNum_Rel > 2*PulseNum_Sum_Buf[i - 1])
                {
                    ACC_Status_Flag = i;
                    DEC_Status_Flag = i;
                    PTO_HZ_NUM_Start(ACC_Status_Flag, PulseNum_Rel);
                    break;
                }
            }
        } 
    }    
}
static void PTO_Cycle(unsigned short Cycle)
{
    TIM_SetAutoreload(TIM1, Cycle - 1);//����TIM1����װֵ
    TIM_SetCompare2(TIM1, Cycle / 2);//����ռ�ձ�Ϊ50%;
    MotorCycleReal = Cycle;    
}
static void PTO_HZ_NUM_Start(unsigned char Flag, signed int pPulseNum_Rel)
{
    PulseNum_ACC = PulseNum_Sum_Buf[Flag - 1]; //
    PulseNum_DEC = PulseNum_ACC;
    PulseNum_ConstantVel = pPulseNum_Rel - (PulseNum_ACC + PulseNum_DEC);
    //MotorFreqU = 1; //ָʾ��Ҫ����
    MotorActionState = 1;
    PTO_Cycle(MotorCycle_Buf[0]);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    TIM_Cmd(TIM1,ENABLE);//ʹ��TIM1 
}

void PTO_Home(u16 Freq)/*���ָ��Ƶ��PWM*/
{
    u16 TIM1per = 0;
    u16 Cycle = 1000000 / Freq;
    TIM1per = Cycle / 10;       //Ԥ��ƵΪ720,10us����һ��
    MotorCycleReal = TIM1per;
    MotorDir = 2;
    GPIO_SetBits(GPIOE, GPIO_Pin_0);//��ת
    PTO_Cycle(TIM1per);
    TIM_Cmd(TIM1,ENABLE);//ʹ��TIM1
}

void PTO_Stop(void)
{
    TIM_Cmd(TIM1, DISABLE);
    TIM_SetCounter(TIM1, 0);  
    MotorCycleReal = 0;                        
    MotorDir = 0;  
    MotorActionState = 0;     //ת��״̬ 0  
}
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
////////////����PWM�����
////////////Freqq:ΪƵ��,��λ(hz)
////////////Pulse_Num:Ϊ�������
//////////void PTO_HZ(u16 Freqq, u16 Pulse_Num)
//////////{
//////////  u16 TIM1per = 0;
//////////  u32 Time = 0;
//////////  u16 Cycle;
//////////  
//////////  Cycle = 1000000 / Freqq;
//////////  //�ı�TIM1����װֵ�ı�����Ƶ��������������ռ�ձ�Ϊ50%
//////////  //�ı�TIM5��Ԥ��Ƶ������װֵ�ı��������
//////////    
//////////  Time = Cycle * Pulse_Num;
//////////  Time /= 100;              //Ԥ��ƵΪ7200,100us����һ��
//////////  TIM1per = Cycle/10;       //Ԥ��ƵΪ720,10us����һ��

//////////  TIM_SetAutoreload(TIM5, Time);//����TIM5����װֵ
//////////  TIM_SetAutoreload(TIM1, TIM1per-1);//����TIM1����װֵ
//////////  TIM_SetCompare2(TIM1,TIM1per/2);//����ռ�ձ�Ϊ50%
//////////  TIM_Cmd(TIM5,ENABLE);//ʹ��TIM5 
//////////  TIM_Cmd(TIM1,ENABLE);//ʹ��TIM1
//////////}


/*******************************************************************/
/***********************�ٶ�ģʽ************************************/
/*******************************************************************/
/*******************************************************************/

////void MotorF(unsigned short Freq, unsigned short Pulse_Num)
////{
////    TIM_Cmd(TIM1,DISABLE);
////    GPIO_SetBits(GPIOE, GPIO_Pin_0);

////    PTOWithAcceleration(Freq);
////}
////void MotorB(unsigned short Freq, unsigned short Pulse_Num)
////{
////    TIM_Cmd(TIM1,DISABLE);
////    GPIO_ResetBits(GPIOE, GPIO_Pin_0);

////    PTOWithAcceleration(Freq);
////}
////void MotorS()
////{
////	PTOStopWithDeceleration();
////}

//////����PWM�����
//////Freq:HZ PWM��� ������  500hz  /  20ms
////void PTOWithAcceleration(u16 Freq)
////{
////	MotorFreqU = 1;	/*Ƶ�ʼ���������־*/
////    MotorFreqFlag = Freq / ACC_HZ_MIN;	/*����Ƶ�� ACC_HZ_MIN ����*/
////    MotorFreqBuf  = Freq % ACC_HZ_MIN;	/*����Ƶ�� ACC_HZ_MIN ����*/
////    if(MotorFreqFlag != 0)/*���Ƶ��ֵ���ڵ���ACC_HZ_MIN*/
////    {
////		PTO_HZ_Period(ACC_HZ_MIN, ACC_PERIOD); /*�������ACC_HZ_MINHZ ���� ��ʼƵ��ΪACC_HZ_MIN HZ*/ 	          
////    }
////    else
////	{ 
////        PTO_HZ_NONum(MotorFreqBuf);	/*���Ƶ��С�� 500HZ ֱ�����ָ��Ƶ��*/
////	}
////}

/////*PWM��� ����Ƶ�� ��DEC_HZ_MIN HZ/ period MS ��С��0*/
////void PTOStopWithDeceleration(void)
////{
////	MotorFreqD = 1;	/*�������ٹ��̱�־*/
////	MotorFreqFlag = MotorFreqNow / DEC_HZ_MIN;

////	if(MotorFreqFlag != 0) /*�����ǰƵ�ʴ���500*/
////	{
////	    PTO_HZ_Period(MotorFreqNow - DEC_HZ_MIN, DEC_PERIOD);/*��� (MotorFreqNow - 500) 20MS*/
////	}
////	else
////	{
////		PTO_Stop();
////	}
////}



/////*pwm��� period ms��ʱ�ж� ָ��Ƶ��PWM���*/
////void PTO_HZ_Period(unsigned short Freq,unsigned short period)
////{
////	u16 TIM1per = 0;
////	u16 Cycle;
////	MotorFreqNow = Freq;	  /*��ǰƵ�ʱ��� ��ֵ*/

////	Cycle = 1000000 / Freq;
////	//�ı�TIM1����װֵ�ı�����Ƶ��������������ռ�ձ�Ϊ50%

////	TIM1per = Cycle / 10;       //Ԥ��ƵΪ720,10us����һ��
////	
////	TIM_SetAutoreload(TIM5, period * 10);//����TIM5����װֵ		1000 * 100us = 100ms
////	TIM_SetAutoreload(TIM1, TIM1per-1);//����TIM1����װֵ
////	TIM_SetCompare2(TIM1,TIM1per/2);//����ռ�ձ�Ϊ50%
////    TIM_Cmd(TIM1,ENABLE);//ʹ��TIM1
////    TIM_Cmd(TIM5,ENABLE);//ʹ��TIM5 
////}


////////��ʱ��5�жϷ������	 
////void TIM5_IRQHandler(void)
////{ 	
////   
////    static u8 FreqFlag;
////	OSIntEnter();
////    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
////	{
////		/*���ٴ������ ACCC_HZ_MIN�������� �ﵽ ACC_HZ_MIN * (Freq / ACC_HZ_MIN) ������MotorFreqBuf ���ﵽ�趨Ƶ��ֵ*/
////        /*�ж��Ƿ������������ MotorFreqU ��PTOWithAcceleration�� ��ֵΪ1������趨Ƶ��ֵ���ڼ��ٶ�ACC_HZ_MIN HZ�� ��ʾ����Ƶ�ʼ��ٹ��� */
////		if(MotorFreqU == 1)
////        {
////            MotorFreqU = 2; /*��ֵΪ2 ��ʾ������ٹ�*/
////            FreqFlag = 0;	/*Ƶ�ʼ����м���� �趨��ֵ 0*/
////        }
////        if(MotorFreqU == 2)	/*����Ƶ�ʼ��ٴ���*/
////        {
////			TIM_Cmd(TIM1,DISABLE);           
////			if(FreqFlag < MotorFreqFlag)/*Ƶ�ʼ����м����С�� MotorFreqFlag */
////            {

////				if(FreqFlag != (MotorFreqFlag -1))/*λ��[0, MotorFreqFlag-1 ) ֮�� ������MotorFreqFlag-1*/
////				{
////		            PTO_HZ_Period(ACC_HZ_MIN * (FreqFlag + 2), ACC_PERIOD); /*���Ƶ��Ϊ 500 * (FreqFlag + 2) PWM  period ms */

////				} 
////			    else if(FreqFlag == (MotorFreqFlag - 1))/*���� MotorFreqFlag - 1 ʱ �����ٽ�״̬ ��������*/
////		        {
////		            
////		            PTO_HZ_NONum(ACC_HZ_MIN * (FreqFlag + 1) +  MotorFreqBuf);/*�������Ƶ��PWM ȡ����ʱ*/
////					MotorFreqBuf = 0; /*�ָ���ֵ*/
////					MotorFreqFlag = 0;/*�ָ���ֵ*/
////		            MotorFreqU = 0;/*���ٱ�־�趨Ϊ0 ��ʾ�������*/
////		            FreqFlag = 0; /*�ָ��м������ֵ*/
////                    
////                    OSSemPost(StepSem);
////		        }
////				FreqFlag ++; 
////            }

////        }
////		/*���ٴ�����̽���*/
////        
////		/*���ٴ������*/
////		 if(MotorFreqD == 1)
////		 {
////		     MotorFreqD = 2;
////			 FreqFlag = 0;
////		 }
////		 if(MotorFreqD == 2)
////		 {
////		     TIM_Cmd(TIM1,DISABLE);
////			 if(FreqFlag < MotorFreqFlag)
////			 {  
////		     	if(FreqFlag != (MotorFreqFlag - 1))
////			 	{
////			 		PTO_HZ_Period(MotorFreqNow - DEC_HZ_MIN, DEC_PERIOD);
////			 	}
////			 	else if(FreqFlag == (MotorFreqFlag - 1))
////				{
////					PTO_Stop();
////				    TIM_Cmd(TIM5,DISABLE);//�رն�ʱ
////					
////					MotorFreqD = 0;
////					MotorFreqFlag = 0;/*�ָ���ֵ*/
////		            FreqFlag = 0; /*�ָ��м������ֵ*/
////				}
////				FreqFlag ++; 
////		     }
////		 }
////	} 
////	TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
////    OSIntExit();    
////}
////��ʱ��1�����жϷ������	 
void TIM1_UP_IRQHandler(void)
{ 	
    static u8 tempflag = 0;

    OSIntEnter();

    if(MotorDir == 1)
        PulseNum_Global++;
    else if(MotorDir == 2)
        PulseNum_Global--;
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{        
        switch(MotorActionState)
        {
            case 1://��������״̬ 
                tempflag = 0;	/*�����м���� �趨��ֵ 0*/
                PulseNum_TEMP =(MotorDir > 1)? (PulseNum_Global +1):(PulseNum_Global - 1);/*��¼��ʼ���������ֵ*/
                MotorActionState = 2;//ת��״̬ 2 ���ٹ�����
            break;
            case 2://���ٹ��� ����
                if((u32)abs(PulseNum_Global - PulseNum_TEMP) > (PulseNum_Buf[tempflag] - 1)) //����ﵽָ��������
                {
                    PulseNum_TEMP = PulseNum_Global;/*��¼��ʼ���������ֵ*/
                    tempflag++;  
                    if(tempflag == ACC_Status_Flag) // 
                    {                        
                        if(PulseNum_ConstantVel == 0) /*����趨����������2���ļӼ�������������� �����ٶ���������Ϊ 0 �������ֱ�ӽ�����ٹ���*/
                        {
                            tempflag = 0;
                            MotorActionState = 4;//��������״̬ 
                            break;                            
                        }
                        else
                        {
                            MotorActionState = 3;//������ٶ�������
                            PTO_Cycle(MotorCycleTarget);
                            break;                            
                        }              
                    }
                    if(tempflag < ACC_Status_Flag) //δ�ﵽĿ��Ƶ�� ��������
                    {
                        PTO_Cycle(MotorCycle_Buf[tempflag]);
                    }
                }
                break;
            case 3://���ٶ��� 
                if((u32)abs(PulseNum_Global - PulseNum_TEMP) > (PulseNum_ConstantVel - 1))//����ﵽָ��������
                {
                    tempflag = 0;
                    MotorActionState = 4;  //ת��״̬ 4 ��Ҫ�������
                }
                break;
            case 4://��������
                tempflag = DEC_Status_Flag;	/*�����м���� �趨��ֵ 0*/
                PulseNum_TEMP =(MotorDir > 1)? (PulseNum_Global +1):(PulseNum_Global - 1);               
                PTO_Cycle(MotorCycle_Buf[tempflag - 1]); 
                MotorActionState = 5; //ת��״̬5 ���ٹ�����           
                break;
            case 5://���ٹ�����
                if((u32)abs(PulseNum_Global - PulseNum_TEMP) > ((PulseNum_Buf[tempflag -1] - 1)))//�ﵽָ��������
                {
                    PulseNum_TEMP = PulseNum_Global;/*��¼����ֵ*/
                    tempflag--;
                    if(tempflag == 0) //�ﵽ0
                    {
                        PTO_Stop();                   
                    }
                    if(tempflag > 0) 
                    {
                        PTO_Cycle(MotorCycle_Buf[tempflag - 1]);
                    }            
                } 
                break;
            default:
                break;
        }
    }
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
    OSIntExit();
}

////////////��ʱ��1�����жϷ������	 
////////void TIM1_UP_IRQHandler(void)
////////{ 	
////////    static u8 tempflag = 0;

////////    OSIntEnter();

////////    if(MotorDir == 1)
////////        PulseNum_Global++;
////////    else if(MotorDir == 2)
////////        PulseNum_Global--;
////////    if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
////////	{
////////        /*���ٴ������*/
////////        if(MotorFreqU == 1)
////////        {
////////            MotorFreqU = 2; /*��ֵΪ2 ��ʾ������ٹ�*/
////////            tempflag = 0;	/*�����м���� �趨��ֵ 0*/
////////            PulseNum_TEMP =(MotorDir > 1)? (PulseNum_Global +1):(PulseNum_Global - 1);/*��¼��ʼ���������ֵ*/
////////        }  
////////        if((MotorFreqU == 2) && ((u32)abs(PulseNum_Global - PulseNum_TEMP) > (PulseNum_Buf[tempflag] - 1)))
////////        {
////////            PulseNum_TEMP = PulseNum_Global;/*��¼��ʼ���������ֵ*/
////////            tempflag++;
////////            if(tempflag == ACC_Status_Flag) //Ŀ��Ƶ�� 
////////            {
////////                PTO_Cycle(MotorCycleTarget);
////////                if(PulseNum_ConstantVel == 0) /*����趨����������2���ļӼ�������������� �����ٶ���������Ϊ 0 �������ֱ�ӽ�����ٹ���*/
////////                {
////////                    tempflag = 0;
////////                    MotorConstantVel = 2;//�������ٶ���  AA
////////                    MotorFreqD = 1;
////////                }
////////                else
////////                {
////////                    MotorConstantVel = 1;//������ٶ�������              
////////                }
////////                MotorFreqU = 0;      //������ٱ�־λ ��־���ٹ������                
////////            }
////////            if(tempflag < ACC_Status_Flag)
////////            {
////////                PTO_Cycle(MotorCycle_Buf[tempflag]);
////////            }
////////        }       
////////        /*���ٽ���*/
////////        
////////        /*���ٶ�������*/
////////        if((MotorConstantVel == 1) && ((u32)abs(PulseNum_Global - PulseNum_TEMP) > (PulseNum_ConstantVel - 1)))
////////        {
////////            tempflag = 0;
////////           // MotorConstantVel = 0;  //AB
////////            MotorFreqD = 1;
////////        }  
////////        
////////        /*���ٴ������*/
////////        if(MotorFreqD == 1)
////////        {
////////            if((MotorConstantVel != 1) && (MotorConstantVel != 2))/*��AA��AB��˳��ִ�е��˴�ʱ ��Ҫ��һ�θ����жϽ��м��ٴ���*/
////////            {
////////                MotorFreqD = 2; /*��ֵΪ2 ��ʾ������ٹ���*/
////////                tempflag = DEC_Status_Flag;	/*�����м���� �趨��ֵ 0*/
////////                PulseNum_TEMP =(MotorDir > 1)? (PulseNum_Global +1):(PulseNum_Global - 1);               
////////                PTO_Cycle(MotorCycle_Buf[tempflag - 1]);               
////////            }
////////            else 
////////            {
////////                MotorConstantVel = 0;
////////            }
////////        } 
////////        if((MotorFreqD == 2) && ((u32)abs(PulseNum_Global - PulseNum_TEMP) > ((PulseNum_Buf[tempflag -1] - 1))))
////////        {
////////            PulseNum_TEMP = PulseNum_Global;/*��¼��ʼ���������ֵ*/
////////            tempflag--;
////////            if(tempflag == 0) //Ŀ��Ƶ�� 
////////            {
////////                TIM_Cmd(TIM1, DISABLE);
////////                TIM_SetCounter(TIM1, 0);
////////                MotorFreqD = 0;      //������ٱ�־λ ��־���ٹ������     
////////                MotorDir = 0;               
////////            }
////////            if(tempflag > 0)
////////            {
////////                PTO_Cycle(MotorCycle_Buf[tempflag - 1]);
////////            }            
////////        }            
////////        
////////        /**/        
////////    }
////////    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
////////    OSIntExit();
////////}
//�ⲿ�ж�1������
void EXTI1_IRQHandler(void)
{
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        PulseNum_Global = 0; 
        if(HomeFlag == 0)
        {
            PTO_Stop();
            HomeFlag = 1;
        }  
  
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
	OSIntExit();
}


void EXTI2_IRQHandler(void)
{
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {

        //EXTI2_OFF;
        
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
	OSIntExit();
}
//�ⲿ�ж�4������
void EXTI4_IRQHandler(void)
{
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        PTO_Stop();
        PulseNum_Global = 0; 
        if(HomeFlag == 0)
        {           
            HomeFlag = 1;
        }    
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
	OSIntExit();
}
