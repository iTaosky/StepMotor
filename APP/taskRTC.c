#include "includes.h"

struct tm time_now = {0, 02, 17, 27, 3, 2015};
//DWORD ttime;
OS_EVENT *RTCMutex; /*time_now �������� �����ź�*/

void vTaskRTC(void *p_arg)
{   
    u8 err;
    RTCMutex = OSMutexCreate(TASK_PIP_PRIO, &err);  /*���������ź���*/
    if(err != OS_ERR_NONE)
    {
        printf("RTCMutex create failure  err = %d\n", err);
    }
    
	vRTCTime_SetCalendarTime(time_now);   /*�趨��ʼʱ��*/
	for(;;)
	{
        OSMutexPend(RTCMutex, 0, &err);   /*��ȡ������*/   
        time_now = xRTCTimeConvUnixToCalendar(u32RTCTime_GetUnixTime());
        OSMutexPost(RTCMutex);            /*�ͷŻ�����*/  
		//printf("\r\nTime:%d-%d-%d,%d:%d:%d\r\n", time_now.tm_year, time_now.tm_mon, time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);       
	}	
}
