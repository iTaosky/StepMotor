#ifndef __BSP_CFG_H__
#define __BSP_CFG_H__
//�ж����ȼ�����

#define SR04_EXTI_PRIO     2       //SR04 ECHO�ز������ж�          �ⲿ�ж�
#define ADC_DMA_PRIO       5       //ADC   DMA ͨ��ת������ж�     DMAͨ���ж�
#define UART_RX_PRIO       3        //UART�����ж�                   UART�ж�
#define LWIP_TIM_PRIO      6        //���ڵ���lwip�ײ���պ���       ��ʱ���ж� 
#define AM2305_EXTI_PRIO   1         //AM2305 ���ݽ����ж�            �ⲿ�ж�
#define PWM_TIM_PRIO       7         //��ʱ��PWM�����ж�           TIM4
#define PWM_TIM1_PRIO      10

#define LS_EXTI_PRIO 			 9 				//��λ�����ⲿ�ж�
#define LS_EXTI_PRIO1 			 8				//��λ�����ⲿ�ж�


#endif
