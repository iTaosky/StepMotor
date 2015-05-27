#include "includes.h"

extern struct tm time_now;
extern vu8 fSR04Distance;
extern float Current_Temp; 

#define lenofstr1 32
#define times     16

FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc, fdst;      /* file objects */
FRESULT res;
UINT br,bw;
//char path0[512]="0:";
char databuf[512];
//char buffer[4096];   /* file copy buffer */

char str1[] = "��:xxx ��:xx.xx Time:xx:xx:xx\r\n";

/*******************��������*********************/
static void prvNamenchild(char *nameb);
static u8 prvIsnewday(void);
static void prvDataInit(void);            /**���洢�����ʼ��ֵ**/
/********************************************/
void vTaskMSD(void *p_arg)
{    
    OS_CPU_SR cpu_sr;
	vu8 datarptr = 0;
    char namebuffer[20] = "0:/";

	//�����ļ�ϵͳ
	res = f_mount(0, &fs);
	if(res != FR_OK){
		printf("mount filesystem 0 failed : %d\n\r",res);
	}

	/*�ļ�����*/	
    prvNamenchild(namebuffer);
		
	/**���洢�����ʼ��ֵ**/
    prvDataInit();
 
	res = f_open(&fdst, &namebuffer[0], FA_CREATE_ALWAYS | FA_WRITE);
    
	for(;;)
	{
        if(prvIsnewday())   /*�ж��Ƿ���Ҫ�½���һ���ļ� ��һ��һ����*/
        {           		
            f_close(&fdst);/*close file */
            prvDataInit();
            OSTimeDlyHMSM(0, 1, 1, 0);           	
            prvNamenchild(namebuffer); /*�ļ�����*/
			OS_ENTER_CRITICAL();
            res = f_open(&fdst, &namebuffer[0], FA_CREATE_ALWAYS | FA_WRITE);
			OS_EXIT_CRITICAL();
            if(res != FR_OK)
            {
                printf("open file error : %d\n\r",res);
            }
        }
        for(datarptr = 0; datarptr < times; datarptr ++)   /**ѭ����ֵ**/
        {
            /*****���븳ֵ****/
            databuf[3 + datarptr * lenofstr1] = (fSR04Distance / 100) + 0x30;
            databuf[4 + datarptr * lenofstr1] = ((fSR04Distance % 100) / 10) + 0x30;
            databuf[5 + datarptr * lenofstr1] = (fSR04Distance % 10) + 0x30;
            /****�¶ȸ߸�ֵ****/
            databuf[10 + datarptr * lenofstr1] = ((u8)(Current_Temp) / 10 ) + 0x30;
            databuf[11 + datarptr * lenofstr1] = ((u8)(Current_Temp) % 10 ) + 0x30;
            /*****�¶ȵ͸�ֵ****/
            databuf[13 + datarptr * lenofstr1] = ((u8)((u16)(Current_Temp * 100) % 100) / 10 ) + 0x30;
            databuf[14 + datarptr * lenofstr1] = ((u8)((u16)(Current_Temp * 100) % 100) % 10 ) + 0x30;
            /**ʱ�丳ֵ**/
            databuf[21 + datarptr * lenofstr1] = ( time_now.tm_hour / 10 ) + 0x30;
            databuf[22 + datarptr * lenofstr1] = ( time_now.tm_hour % 10 ) + 0x30;
            
            databuf[24 + datarptr * lenofstr1] = ( time_now.tm_min / 10 ) + 0x30;
            databuf[25 + datarptr * lenofstr1] = ( time_now.tm_min % 10 ) + 0x30;
            
            databuf[27 + datarptr * lenofstr1] = ( time_now.tm_sec / 10 ) + 0x30;
            databuf[28 + datarptr * lenofstr1] = ( time_now.tm_sec % 10 ) + 0x30;
            OSTimeDlyHMSM(0, 0, 1, 0);
        }
        if(datarptr == times) /**��������ֵ**/
        {            
            datarptr = 0;
			OS_ENTER_CRITICAL();
            res = f_write(&fdst, databuf, 512, &bw);               /* Write it to the dst file */
            if(res == FR_OK){
                printf("write data ok! %d\n\r",bw);
            }else{
                printf("write data error : %d\n\r",res);
            }
            f_sync (&fdst);                                        /*������д���ļ�����f_close���� �������ڱ����ļ���״̬*/
			OS_EXIT_CRITICAL();
			prvDataInit();
        }

        OSTimeDlyHMSM(0, 0, 1, 0);
		    
	}	
}

/***********����ʱ������ļ�����**************/
void prvNamenchild(char *nameb)
{
  	nameb[3] =  ( (time_now.tm_year / 100) / 10 ) + 0x30;
	nameb[4] =  ( (time_now.tm_year / 100) % 10 ) + 0x30;
    nameb[5] =  ( (time_now.tm_year % 100) / 10 ) + 0x30; 
	nameb[6] =  ( (time_now.tm_year % 100) % 10 ) + 0x30;
	nameb[7] =  ( time_now.tm_mon / 10 ) + 0x30;
	nameb[8] =  ( time_now.tm_mon % 10 ) + 0x30;
    nameb[9] =  ( time_now.tm_mday / 10 ) + 0x30; 
	nameb[10] =  ( time_now.tm_mday % 10 ) + 0x30;
    
	nameb[11] =  '.'; 
	nameb[12] =  't'; 
	nameb[13] =  'x'; 
	nameb[14] =  't'; 
	nameb[15] =  ' ';     
}
/***********�ж��Ƿ�Ϊ�µ�һ�� **************/
u8 prvIsnewday(void)
{
    if((time_now.tm_hour == 23) && (time_now.tm_min == 59))
        return 1;
    else
        return 0;
}
/**���洢�����ʼ��ֵ**/
void prvDataInit(void)
{
    u8 i;
    for(i = 0;i < times; i++)
    {
        strcpy(&databuf[i * lenofstr1], str1);
    }
    for(i = 1;i < times + 1; i++)
    {
        databuf[i * lenofstr1 - 1] = '\r';
    }
}
//	//д�ļ�����
//	printf("write file test......\n\r");
//    res = f_open(&fdst, "0:/hello1114.txt", FA_CREATE_ALWAYS | FA_WRITE);
//	if(res != FR_OK){
//		printf("open file error : %d\n\r",res);
//	}else{
//	    res = f_write(&fdst, textFileBuffer, sizeof(textFileBuffer), &bw);               /* Write it to the dst file */
//		if(res == FR_OK){
//			printf("write data ok! %d\n\r",bw);
//		}else{
//			printf("write data error : %d\n\r",res);
//		}
//        res = f_write(&fdst, textFileBuffer, sizeof(textFileBuffer), &bw);               /* Write it to the dst file */
//		if(res == FR_OK){
//			printf("write data ok! %d\n\r",bw);
//		}else{
//			printf("write data error : %d\n\r",res);
//		}
//         f_printf(&fdst,"julishi:%d",55);
//
//		/*close file */
//		f_close(&fdst);
//	}
