#ifndef __PTO_H__
#define __PTO_H__
void vPTOConfig(void);
void PTO(unsigned short Cycle, unsigned short Pulse_Num);
void MotorF(unsigned short Freq, unsigned short Pulse_Num);
void MotorB(unsigned short Freq, unsigned short Pulse_Num);
void MotorS(void);

void PTOWithAcceleration(unsigned short Freq);	/*�����ٶ�����PWM���  500HZ / 200MS*/
void PTOStopWithDeceleration(void);	/*�����ٶȹر�PWM���  500HZ / 200MS*/

static void PTO_Stop(void);/*�ر�PWM���*/ 

void PTO_HZ_NONum(unsigned short Freq);/*���ָ��Ƶ��PWM ��ʱ������*/
void PTO_HZ_Period(unsigned short Freq,unsigned short period);	/*���20msָ��Ƶ��PWM*/  

void PTO_HZ_NUM(unsigned short Freq, unsigned int Pulse_Num);
static void PTO_Cycle(unsigned short Cycle);
static void PTO_HZ_NUM_Start(unsigned char Flag, signed int pPulseNum_Rel);

#define EXTI1_OFF EXTI->IMR &= (~EXTI_IMR_MR1)
#define EXTI1_ON  EXTI->IMR |= EXTI_IMR_MR1

#define EXTI2_OFF EXTI->IMR &= (~EXTI_IMR_MR2)
#define EXTI2_ON  EXTI->IMR |= EXTI_IMR_MR2

/*Step�Զ�ģʽ��־��*/
#define AutoF 0x01
#define AutoB 0x02
#define AutoS 0x04

/*�Ӽ��ٲ�������
*���ٶ�Ϊ��ACC_HZ_MIN / ACC_PERIOD  HZ/MS 
*/
/*�Ӽ�����Сʱ�䵥λ  ms*/
#define ACC_PERIOD   100
#define DEC_PERIOD   100
/*�Ӽ�����СƵ�ʵ�λ  hz*/
#define ACC_HZ_MIN   200
#define DEC_HZ_MIN   200

#endif
