#ifndef __TASKCOMMUNICATION_H__
#define __TASKCOMMUNICATION_H__

/*HEAD����*/
#define HEAD1       02
#define HEAD2       0xAA

#define END1 	    0x55
#define END2 		0x03

/*ָ���*/
#define Distance    1          //��ȡ����
#define VERSION     2          //��ȡ�汾��
#define Temp        3          //�¶�
#define TimeGet     4          //ʱ��
#define TimeCalibration   5    //ʱ��У׼
#define Lift              6    //����ƽָ̨���
#define StepMotor         7    //���ָ���

/*����ƽָ̨���*/
#define  LiftUp                       0x01      //ƽ̨����                                        
#define  LiftDown					  0x02 		//ƽ̨�½�
#define  LiftStop                     0x04      //ƽֹ̨ͣ
#define  LiftPreSetPosition_Set       0x08      //Ԥ��λ����
#define  LiftPreSetPosition_Invoke    0x10      //Ԥ��λ�õ���
#define  LiftPreSetPosition_Get       0x20      //Ԥ��λ��ȡ
#define  LiftPositionTostore          0x40      //Ԥ��λ�洢

/*���ָ���*/
#define MotorFor                      0x01 //ǰ��
#define MotorBack                     0x02 //����
#define MotorStop                     0x04 //ֹͣ

/*ָ��ȶ���*/
#define LEN         3

#define TimeLen     7

typedef struct 
{
    unsigned char CMD;             /*ָ������*/ 
    unsigned char RDataLen;        /*���ݳ���*/    
    unsigned char FrameLen;        /*���ݳ���*/
    unsigned char CHECK;           /*���У��ֵ*/
    unsigned char FrameRData[10];  /*������������*/
}FrameS;/*Э��������� �ṹ��*/


void vTaskComm(void *p_arg);
static void prvDisReport(void);
static void prvTempReport(void);
static void prvTimeGet(void);
static void prvTimeCalibration(void);
static void prvSendBuFlush(void);
static void prvLift(void);
static void prvMotor(void);
#endif
