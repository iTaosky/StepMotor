#include "includes.h"
#include <stdlib.h>

#define LiftTime_calibration 0 //�ʹ������ݷ���ʱ���й� 7������ ��ʱ7��ʱ�䳣��


OS_FLAG_GRP *LiftStatus;/*����ƽ̨ �¼���־��*/

u8 LiftU[7] = {0xCA, 0x20, 0xF0, 0x21, 0x01, 0x01, 0xAC}; /*��������*/
u8 LiftD[7] = {0xCA, 0x20, 0xF0, 0x21, 0x01, 0x02, 0xAC}; /*�½�����*/
u8 LiftS[7] = {0xCA, 0x20, 0xF0, 0x21, 0x01, 0x00, 0xAC}; /*ֹͣ����*/

s32 PreSet_Position[512];   /*Ԥ��λ ����*/
u16 PreSet_Position_No;     /*Ԥ��λ ��*/
s32 Time_LiftTable;         /*����ƽ̨ ȫ��λ�ñ���*/
const u32 FLASH_PAGE_ADDR = 0x8060800; /*FLASH��ʼ��ַ �洢 Ԥ��λ ���� 2K һҳ*/
void vTaskLIFT(void *p_arg)
{   
    u8  err;
    u16 PreSet_Position_NoBuf;  /*Ԥ��λ�� ����*/
    u32 Time_LiftTable_buf1, Time_LiftTable_buf2, Time_LiftTable_buf; /*λ�ã�ʱ�䣩���� ����*/

    s32 PreSetbuf;              /*Ԥ��λ ���� �м����*/
    OS_FLAGS LiftValues, LiftValuesTemp; /*����ƽ̨ �¼���־���� ��������*/
    LiftValuesTemp = 0;                   
    LiftStatus = OSFlagCreate(0x00, &err); /*�����¼���־��*/
    if(err != OS_ERR_NONE)
    {
        printf("LiftStatus create failure  err = %d\n", err);
    }
    
	for(;;)
	{
        LiftValues = OSFlagPend(LiftStatus, LiftUp + LiftDown + LiftStop + LiftPreSetPosition_Get + LiftPreSetPosition_Invoke + LiftPreSetPosition_Set + LiftPositionTostore,
                                OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);       /*�ȴ��¼���־��*/ 
        if(err == OS_ERR_NONE)
        {
            switch(LiftValues)/*�����¼���־����в����ж�*/
            {
                case LiftUp:
                    if(LiftValuesTemp == LiftDown) /*����ϴβ���Ϊ���¶���  ����ֹͣ���� ��¼��ǰλ��*/
                    {
                        UART4_Sendarray(LiftS, 7);
                        Time_LiftTable_buf2 = OSTimeGet();
                        if(Time_LiftTable_buf2 < Time_LiftTable_buf1)   //�ж�UCOSϵͳʱ���Ƿ񳬹�0xffffffff �������ڼ����ֵʱ  ���ϴ�ֵ
                            Time_LiftTable_buf = Time_LiftTable_buf2 - Time_LiftTable_buf1 + 0xFFFFFFFF;
                        else 
                            Time_LiftTable_buf = Time_LiftTable_buf2 - Time_LiftTable_buf1;
                        Time_LiftTable +=  Time_LiftTable_buf;
                    }
                    else if(LiftValuesTemp == LiftUp)
                    {
                        break;
                    }
                    UART4_Sendarray(LiftU, 7);          /*���Ͷ�������*/
                    Time_LiftTable_buf1 = OSTimeGet();  /*��ȡ��ʼ������ʱ��*/
                    break;
                case LiftDown:
                    if(LiftValuesTemp == LiftUp) /*����ϴβ���Ϊ���϶���  ����ֹͣ���� ��¼��ǰλ��*/
                    {
                        UART4_Sendarray(LiftS, 7);
                        Time_LiftTable_buf2 = OSTimeGet();
                        if(Time_LiftTable_buf2 < Time_LiftTable_buf1) //�ж�UCOSϵͳʱ���Ƿ񳬹�0xffffffff �������ڼ����ֵʱ  ���ϴ�ֵ
                            Time_LiftTable_buf = Time_LiftTable_buf2 - Time_LiftTable_buf1 + 0xFFFFFFFF;
                        else 
                            Time_LiftTable_buf = Time_LiftTable_buf2 - Time_LiftTable_buf1;                        
                        Time_LiftTable -=  Time_LiftTable_buf;
                    }
                    else if(LiftValuesTemp == LiftDown)
                    {
                        break;
                    }
                    UART4_Sendarray(LiftD, 7);       /*���Ͷ�������*/
                    Time_LiftTable_buf1 = OSTimeGet();/*��ȡ��ʼ������ʱ��*/
                    break;
                case LiftStop:
                    UART4_Sendarray(LiftS, 7);      /*����ֹͣ����*/
                    Time_LiftTable_buf2 = OSTimeGet();/*��ȡ����ֹͣ��ʱ��*/
                    /*���㶯��ʱ��*/
                    if(Time_LiftTable_buf2 < Time_LiftTable_buf1)   //�ж�UCOSϵͳʱ���Ƿ񳬹�0xffffffff �������ڼ����ֵʱ  ���ϴ�ֵ
                        Time_LiftTable_buf = Time_LiftTable_buf2 - Time_LiftTable_buf1 + 0xFFFFFFFF;
                    else 
                        Time_LiftTable_buf = Time_LiftTable_buf2 - Time_LiftTable_buf1;
                    /*���ݶ������� ���� ȫ��λ�ñ���*/
                    if(LiftValuesTemp == LiftDown)
                    {                        
                        Time_LiftTable +=  Time_LiftTable_buf;
                    }
                    if(LiftValuesTemp == LiftUp)
                    {                        
                        if(Time_LiftTable <= Time_LiftTable_buf)  /*�ж��Ƿ��ѵ��ϼ��� Ĭ���ϼ���״̬ λ�ñ���Ϊ0*/
                            Time_LiftTable = 0;
                        else
                            Time_LiftTable -=  Time_LiftTable_buf;
                    }
                    printf("��ǰλ��%d\n", Time_LiftTable);
                    break;
                case LiftPreSetPosition_Get:
                    break;
                case LiftPreSetPosition_Invoke:
                    printf("Ԥ��λ%d ����:%d ��ǰλ��%d\n",PreSet_Position_No, PreSet_Position[PreSet_Position_No], Time_LiftTable);
                    PreSetbuf = PreSet_Position[PreSet_Position_No] - Time_LiftTable;  /*������Ҫ�����ľ��루ʱ�䣩*/
                    /*����ʱ������� ���㶯������;��루ʱ�䣩*/
                    if(PreSetbuf > 0)
                    {                
                        UART4_Sendarray(LiftD, 7);
                        Time_LiftTable_buf1 = OSTimeGet(); 
                        OSTimeDly(PreSetbuf - LiftTime_calibration);
                    }
                    else if(PreSetbuf < 0)
                    {                        
                        UART4_Sendarray(LiftU, 7);  
                        Time_LiftTable_buf1 = OSTimeGet(); 
                        OSTimeDly(abs(PreSetbuf) - LiftTime_calibration);                    
                    }
                    /*ʱ�䵽�� ����ֹͣ����*/
                    UART4_Sendarray(LiftS, 7);                    
                    Time_LiftTable_buf2 = OSTimeGet();
                    printf("ʵ�ʶ���ʱ��= %d   ", Time_LiftTable_buf2 - Time_LiftTable_buf1);
                    printf("�����趯��ʱ��= %d\n", abs(Time_LiftTable - PreSet_Position[PreSet_Position_No]));
                    /*�����ǰλ�ñ���*/
                    Time_LiftTable = PreSet_Position[PreSet_Position_No];
                    break;
                case LiftPreSetPosition_Set:
                    if(LiftValuesTemp == LiftStop) /*�ϴν��յ�ָ�����Ϊֹͣ ���ܽ���Ԥ��λ����*/
                    {
                        PreSet_Position[PreSet_Position_No] = Time_LiftTable;
                        printf("Ԥ��λ%d ���óɹ�:%d\n",PreSet_Position_No, PreSet_Position[PreSet_Position_No]);
                    }
                    break;
                case LiftPositionTostore:
                    /*��Ԥ��λ���ݴ���FLASH ��ַΪ FLASH_PAGE_ADDR */
                    PreSet_Position_NoBuf = 0;
                    RCC_HSICmd(ENABLE);    /*����HSI*/
                    FLASH_Unlock();        /*FLASH���ƿ� ����*/
                    /*���һЩ��־λ*/
                    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
                    /*���� FLASH_PAGE_ADDR��ʼ��FLASH ҳ*/
                    FLASH_ErasePage(FLASH_PAGE_ADDR);
                    do/*д������ 2K*/
                    {
                        FLASH_ProgramWord((FLASH_PAGE_ADDR + PreSet_Position_NoBuf * 4), PreSet_Position[PreSet_Position_NoBuf]);
                        PreSet_Position_NoBuf++;
                    }
                    while(PreSet_Position_NoBuf != 256);
                    FLASH_Lock();/*����FLASH���ƿ�*/
                    break;
                default:
                    break;
            }
            if(Time_LiftTable < 0)
                Time_LiftTable = 0;
            LiftValuesTemp = LiftValues;
        }
	}	
}
