#include "includes.h"

void vRTCConfig(void)
{
//    NVIC_InitTypeDef NVIC_InitStructure;
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE); /*��BKP PWRʱ��*/

//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	PWR_BackupAccessCmd(ENABLE);     /*ʹ��RTC�ͺ󱸼Ĵ�������*/
	BKP_DeInit();                     /*��λ���ݼĴ�������*/
	RCC_LSEConfig(RCC_LSE_ON);       /*����LSE*/
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);          /*�ȴ�LSE����*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                       /*ѡ��LSEΪRTCʱ��Դ*/
	RCC_RTCCLKCmd(ENABLE);                                        /*ʹ��RTCʱ��*/
    RTC_WaitForSynchro();                                         /*�ȴ�RTC�Ĵ���ͬ�����*/
    RTC_WaitForLastTask();										/*�ȴ���� һ�ζ�RTC�Ĵ�����д�������*/
//	RTC_ITConfig(RTC_IT_SEC, ENABLE);                             /*ʹ��RTC���ж�*/
    RTC_WaitForLastTask();										/*�ȴ���� һ�ζ�RTC�Ĵ�����д�������*/
    RTC_SetPrescaler(32767);                                    /*����RTCʱ�ӷ�Ƶ 32767 ����Ƶ��Ϊ 32.768kHz / (32767+1) = 1Hz*/
    RTC_WaitForLastTask();										/*�ȴ���� һ�ζ�RTC�Ĵ�����д�������*/

}
/*�������Ĺ�Ԫ��ʽʱ��ת����UNIXʱ���ʽ*/
void      vRTCTime_SetCalendarTime(struct tm t)    
{
    vRTCTime_SetUnixTime(vRTCTime_ConvCalendarToUnix(t));
	//return;
}
/*д��RTCʱ�ӵ�ǰʱ��*/
u32     vRTCTime_ConvCalendarToUnix(struct tm t)        
{
    t.tm_year -= 1900;
	return mktime(&t);
}
/*ת��UNIXʱ���Ϊ����ʱ��*/
struct tm xRTCTimeConvUnixToCalendar(time_t t)      
{
    struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm -> tm_year += 1900;
	return *t_tm;
}
 /*��RTC��ȡ��ǰUNIXʱ���ʽ��ʱ��ֵ*/
u32       u32RTCTime_GetUnixTime(void)            
{
    return (u32)RTC_GetCounter();
}
void      vRTCTime_SetUnixTime(time_t t)           /*��������Unixʱ���д��RTC*/
{
    RTC_WaitForLastTask();										/*�ȴ���� һ�ζ�RTC�Ĵ�����д�������*/
    RTC_SetCounter((u32)t);
    RTC_WaitForLastTask();										/*�ȴ���� һ�ζ�RTC�Ĵ�����д�������*/
    //return;
}
