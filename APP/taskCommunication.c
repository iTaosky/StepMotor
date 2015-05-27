#include "includes.h"

/********����ȫ�ֱ���**********/
extern vu8 fSR04Distance;
extern float Current_Temp; 
extern struct tm time_now;
extern OS_EVENT *RTCMutex;            /*time_now �������� �����ź�*/
extern OS_FLAG_GRP *LiftStatus;       /*Lift���� ״̬��־��*/
extern u16 PreSet_Position_No;

/******************************/

FrameS xFrame;

/********���ݴ�������**********/
u8 u8SendBuf[20];
/*******����ָ��ӳ������*******/
void  (*pf[8])(void) =
{   0,             /*0:δ����*/
    prvDisReport,   /*Distance(1):�����ϱ�*/
    0, 
    prvTempReport,   /*Temp(3):оƬ�¶Ȳ�ѯ*/
    prvTimeGet,       /*Timeget(4):ʱ���ѯ*/
    prvTimeCalibration,  /*TimeCal(5):ʱ��У׼*/
    prvLift,
    prvMotor
};
/******************************/

void vTaskComm(void *p_arg)
{
	static u8 State = 0;
	static u8 i = 0;
    u8 data;

	for(;;)
    {
        if(FrameBufRead(&data))
        {
            switch(State)
            {
                case 0:
                    if(data == HEAD1)      /*�ж�֡ͷHEAD1*/
                        State = 1;
                    else
                        State = 0;                 
                    break;
                case 1:
                    if(data == HEAD2)      /*�ж�֡ͷHEAD2*/
                    {	
                        i = 0;
                        xFrame.CHECK = 0;
                        State = 2;
                    }
                    else
                        State = 0;
                    break;
                case 2:                   /*�ж�ָ��*/
                    if((data == Distance) || (data == VERSION) || (data == Temp) || (data == TimeGet) || (data == TimeCalibration) || (data == Lift) || (data == StepMotor))
                    {
                        xFrame.CMD = data;
                        xFrame.CHECK ^= data;
                        State = 3;
                    }
                    else
                        State = 0;
                    break;
                case 3:                                /*���ݳ��� �ڣ�0~10��֮�� �������ݽ�����*/
                    if((data > 0) && (data < 10))
                    {
                        xFrame.RDataLen = data;
                        xFrame.CHECK ^= data;
                        State = 4;
                    }
                    else if(data == 0)                 /*���ݳ���Ϊ0ʱ ����������*/
                     {
                        xFrame.RDataLen = data;
                        xFrame.CHECK ^= data;
                        State = 5;
                    }                       
                    else 
                        State = 0;
                    break;
                case 4:
                    if( i < xFrame.RDataLen )           /*���ݽ���*/
                    {
                        xFrame.FrameRData[i] = data;
                        xFrame.CHECK ^= data;
                        i++;
                    }
                    if( i == xFrame.RDataLen )
                    {
                        State = 5;
                        i = 0;
                    }
                    break;
                case 5:                                /*У��Ƚ�*/
                    if(data == xFrame.CHECK)
                        State = 6;
                    else
                        State = 0;
                    break;
                case 6:
                    if(data == END1)
                        State = 7;
                    else
                        State = 0;				
                    break;
                case 7:
                    if(data == END2)
                    {
                        pf[xFrame.CMD]();             /*���ô�����*/
                        State = 0;
                    }
                    else
                        State = 0;
                    break;

                default:
                    break;
            }
        }
        else 
            OSTimeDlyHMSM(0, 0, 0, 60);      /**/ 
//		lwip_pkt_handle();
//		OSTimeDlyHMSM(0, 0, 0, 60);      /**/
              
    }
}
/**********�����ϱ�*************/
void prvDisReport(void)
{
    u8 i = 0;
    prvSendBuFlush();
    u8SendBuf[0] = HEAD1;
    u8SendBuf[1] = HEAD2;
    u8SendBuf[2] = xFrame.CMD;
    
    u8SendBuf[3] = 1;
    u8SendBuf[4] = (u8)fSR04Distance;
    for(i= 2; i < 5; i++)
        u8SendBuf[5] ^= u8SendBuf[i];
    u8SendBuf[6] = END1;
    u8SendBuf[7] = END2;
    UART1_Sendarray(u8SendBuf, 8);
    
}
/**********�¶��ϱ�*************/
void prvTempReport(void)
{
    u8 i = 0;
    prvSendBuFlush();
    u8SendBuf[0] = HEAD1;
    u8SendBuf[1] = HEAD2;
    u8SendBuf[2] = xFrame.CMD;
    
    u8SendBuf[3] = 2;
    u8SendBuf[4] = (u8)(Current_Temp);
    u8SendBuf[5] = (u8)((u16)(Current_Temp * 100) % 100);
    for(i= 2; i < 6; i++)
        u8SendBuf[6] ^= u8SendBuf[i];
    u8SendBuf[7] = END1;
    u8SendBuf[8] = END2;
    UART1_Sendarray(u8SendBuf, 9);    
}
/*********ʱ���ϱ�*************/

void prvTimeGet()
{
    u8 i = 0;
    u8 err;
    
	prvSendBuFlush();
    u8SendBuf[0] = HEAD1;
    u8SendBuf[1] = HEAD2;
    u8SendBuf[2] = xFrame.CMD;
    
    u8SendBuf[3]  = 7;
    OSMutexPend(RTCMutex, 0, &err);   /*�ͷŻ�����*/ 
	u8SendBuf[4]  = time_now.tm_year / 100;
	u8SendBuf[5]  = time_now.tm_year % 100;

	u8SendBuf[6]  = time_now.tm_mon;
	u8SendBuf[7]  = time_now.tm_mday;
	u8SendBuf[8]  = time_now.tm_hour;
	u8SendBuf[9]  = time_now.tm_min;
	u8SendBuf[10] = time_now.tm_sec;
    OSMutexPost(RTCMutex);            /*�ͷŻ�����*/ 
	for(i = 2; i < 11; i++)
		u8SendBuf[11] ^= u8SendBuf[i];
    
    u8SendBuf[12] = END1;
    u8SendBuf[13] = END2;
    
	UART1_Sendarray(u8SendBuf, 14); 	
}
/*********ʱ��У׼*************/
void prvTimeCalibration()
{
    u8 err;
    
    OSMutexPend(RTCMutex, 0, &err);    /*��ȡ������*/  
    time_now.tm_year = (xFrame.FrameRData[0] * 100) + xFrame.FrameRData[1];
	time_now.tm_mon  = xFrame.FrameRData[2];
	time_now.tm_mday  = xFrame.FrameRData[3];
	time_now.tm_hour  = xFrame.FrameRData[4];
	time_now.tm_min  = xFrame.FrameRData[5];
	time_now.tm_sec  = xFrame.FrameRData[6];

	vRTCTime_SetCalendarTime(time_now);
    OSMutexPost(RTCMutex);            /*�ͷŻ�����*/ 
    
    prvSendBuFlush();
    u8SendBuf[0] = HEAD1;
    u8SendBuf[1] = HEAD2;
    u8SendBuf[2] = xFrame.CMD;
    u8SendBuf[3] = 0;
    u8SendBuf[4] = u8SendBuf[2];
    u8SendBuf[5] = END1;
    u8SendBuf[6] = END2;
    
	UART1_Sendarray(u8SendBuf, 7); 
}
/*********����ƽ̨����*************/
void prvLift(void)
{
    u8 err;
    if(xFrame.RDataLen == 3)
        PreSet_Position_No = (u16)((u16)(xFrame.FrameRData[1] << 8) | xFrame.FrameRData[2]); 
    OSFlagPost(LiftStatus, xFrame.FrameRData[0], OS_FLAG_SET, &err);
    
}

/*************�����������***************/
void prvMotor()
{
//    unsigned short Pulse_Num, Freq;
//    if(xFrame.RDataLen == 5)
//    {
//        Freq  = (xFrame.FrameRData[1] << 8) | (xFrame.FrameRData[2]);

//        Pulse_Num = (xFrame.FrameRData[3] << 8) | (xFrame.FrameRData[4]);
//    }
//    switch(xFrame.FrameRData[0])
//    {
//        case MotorFor:
//            MotorF(Freq, Pulse_Num);
//            break;
//        case MotorBack:
//            MotorB(Freq, Pulse_Num);
//            break;
//        case MotorStop:
//            MotorS();
//            break;
//        default:
//            break;
//    }        
}
void prvSendBuFlush()
{
    u8 i= 0;
    for(i = 0;i < 20; i++)
    {
        u8SendBuf[i] = 0;    
    }
}
//time_now.tm_year, time_now.tm_mon, time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec



