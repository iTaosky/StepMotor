#include "includes.h"

extern u16 MotorFreqNow;

u8 AutoModeFlag = 0; //1���Զ�ģʽ  0��ָ��ģʽ
////OS_EVENT *StepSem; /*Ƶ�ʼ�������ź���*/
////OS_FLAG_GRP *StepStatus;/*Step�Զ�ģʽ �¼���־��*/
void vTaskStepMotor(void *p_arg)
{
    u8 err;
    OS_FLAGS StepValues;
    
    
//    StepStatus = OSFlagCreate(0x00, &err); /*�����¼���־��*/
//    StepSem = OSSemCreate(0);
//    if(err != OS_ERR_NONE)
//    {
//        printf("StepStatus create failure  err = %d\n", err);
//    }
    
     
//    OSTimeDlyHMSM(0, 0, 5, 0);
//    
//    PTO_HZ_NUM(1600, 6400);  
//    OSTimeDlyHMSM(0, 0, 5, 0);
//    PTO_HZ_NUM(3200, 16000);  
//    OSTimeDlyHMSM(0, 0, 8, 0);
//    PTO_HZ_NUM(6400, 32000);  
//    OSTimeDlyHMSM(0, 0, 8, 0);
//    PTO_HZ_NUM(7800, 78000);  
//    OSTimeDlyHMSM(0, 0, 20, 0);
//    //PTO_HZ_NUM(241, 700); 
//    PTO_HZ_NUM(6400, 6400);  
//    OSTimeDlyHMSM(0, 0, 15, 0);   
//    PTO_HZ_NUM(1600, 0);  
//    OSTimeDlyHMSM(0, 0, 8, 0);

//    
//    OSSemPend(StepSem, 0, &err);  //��APP.c����һ���ٺ������ã���Ҫ�ڴ˴������ź�������
    for(;;)
    {
////        StepValues = OSFlagPend(StepStatus, AutoF + AutoB, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);       /*�ȴ��¼���־��*/ 
////        if(err == OS_ERR_NONE)
////        {
////            switch(StepValues)
////            {
////                case AutoF:               
////                    MotorS();
////                    OSTimeDlyHMSM(0, 0, 2, 0); 
////                    while(MotorFreqNow != 0);
////                    MotorF(3200,0); 
////                    OSSemPend(StepSem, 0, &err);
////                    EXTI1_ON;
////                    break;
////                case AutoB:
////                    MotorS();
////                    OSTimeDlyHMSM(0, 0, 2, 0); 
////                    while(MotorFreqNow != 0);
////                    MotorB(3200,0);
////                    OSSemPend(StepSem, 0, &err);
////                    EXTI2_ON;
////                    break;
////                default:
////                    break;
////            }
////        }
//        while(AutoModeFlag)
//        {
//                    
//        }
    }
}
