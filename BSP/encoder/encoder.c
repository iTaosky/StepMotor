#include "includes.h" 

/*������������� ������ֵ ��Զ����ﵽ*/
#define  MAX_COUNT  0x5000

u8  ENCODERDIR_FLAG; /*�����־λ��0 ���� 1 ����*/

s32 nowENCODERCNT;  /*��ǰ������λ�� *0.02mm ��Ϊ��ǰ���߱������������*/

void vENCODERConfig()
{
  prvTIM8_encoder();
} 

signed int s32GetEncoderCNT(void)
{
    static s16 preTIM8CNT; /*���徲̬���� �洢��һ�μ���ֵ*/
    s16 nowTIM8CNT;        /*�洢��ǰ����ֵ*/
    s32 dTIM8CNT;          /*�洢10ms����� ��������*/
    nowTIM8CNT = TIM_GetCounter(TIM8); /*��ȡ��ǰ����ֵ*/
    dTIM8CNT = nowTIM8CNT - preTIM8CNT;/*���㵱ǰ����һ�μ�����ֵ*/
    
    if(dTIM8CNT > MAX_COUNT )   /*���¼��� ���*/         
    {
        dTIM8CNT -= 0x10000;
    }
    else if(dTIM8CNT < -MAX_COUNT) /*���ϼ��� ���*/
    {
        dTIM8CNT += 0x10000;
    }
    preTIM8CNT = nowTIM8CNT;/*�洢��ǰ����ֵ*/
    
    /*TIMx_CR1 λ4 Ϊ�������� 0��up  1: down*/
    ENCODERDIR_FLAG = ((TIM8->CR1 >> 4) & 0x01) ;
    
    return dTIM8CNT;/*����10ms�ڶ�������ֵ*/
        
}
/*TIM8 ������ģʽ��ʼ��*/
void prvTIM8_encoder(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	/*�������ʱ�� TIM8 PC*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE); //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //
	/*PC6 PC7��ʼ��*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);  //
	/*TIM8 ʱ����ʼ�� ���ֵΪ0xFFFF ���ϼ���*/
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); // Time base configuration
    
    /*��ʼ��TIM8������ģʽ3 TI1FP1 TI2FP2 �����ؼ���*/
	TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, 
				                     TIM_ICPolarity_Rising, 
									 TIM_ICPolarity_Rising);
    /*��ʼ��ͨ��1 2 ʹ�����벶��1 �˲�*/                                
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2 ;
	TIM_ICInitStructure.TIM_ICFilter = 6;//ICx_FILTER;
	TIM_ICInit(TIM8, &TIM_ICInitStructure);

	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM8, 0);	//???????   
	TIM_Cmd(TIM8, ENABLE);
}


 
