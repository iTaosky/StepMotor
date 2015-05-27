#ifndef __RTC_H__
#define __RTC_H__
#include "includes.h"

void vRTCConfig(void); /*��ʼ������ ��ϵͳ����ʱ����*/

/*UNIXʱ����غ���*/
void      vRTCTime_SetCalendarTime(struct tm t);     /*�������Ĺ�Ԫ��ʽʱ��ת����UNIXʱ���ʽ*/
struct tm xRTCTimeConvUnixToCalendar(time_t t);      /*ת��UNIXʱ���Ϊ����ʱ��*/


static u32      vRTCTime_ConvCalendarToUnix(struct tm t);  /*д��RTCʱ�ӵ�ǰʱ��*/
u32       u32RTCTime_GetUnixTime(void);              /*��RTC��ȡ��ǰUNIXʱ���ʽ��ʱ��ֵ*/
static void      vRTCTime_SetUnixTime(time_t t);            /*��������Unixʱ���д��RTC*/
#endif
