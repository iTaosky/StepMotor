#include "includes.h"
#define  ADC1_DR_Address    ((u32)0x40012400+0x4c) 
vu16 AD_Value[1];   //
float Current_Temp; 

void vADCConfig(void)
{
    /*SR04      �ܽ�����
	*TRIG(PC8)  �������
	*ECHO(PC7)  ��������  �ⲿ�ж�
    */
    
    ADC_InitTypeDef ADC_InitStructure;   
	 /*����ʱ��ADC1,DMA1*/
	//����DMAʱ�� 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);   
    //����ADC1ʱ��    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
   
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ת��ģʽΪ���������н���ȷǳ�������ѡ��   
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;   
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //����ת������    
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   
    ADC_InitStructure.ADC_NbrOfChannel = 1;     //����ת�����г���Ϊ1   
    ADC_Init(ADC1, &ADC_InitStructure);   
      
    //ADC�����¶ȴ�����ʹ�ܣ�Ҫʹ��Ƭ���¶ȴ��������м�Ҫ��������    
    ADC_TempSensorVrefintCmd(ENABLE);   
       
    //����ת������1��ͨ��16���ڲ��¶ȴ�������������ʱ��>2.2us,(239cycles)    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5); 

     //���������ADC���裬ADCͨ����ת������˳�򣬲���ʱ�� 
    // Enable ADC1    
    ADC_Cmd(ADC1, ENABLE);   
    // ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����    
    ADC_DMACmd(ADC1, ENABLE);   
      
    // ������ADC�Զ�У׼����������ִ��һ�Σ���֤����    
    // Enable ADC1 reset calibaration register    
    ADC_ResetCalibration(ADC1);   
    // Check the end of ADC1 reset calibration register    
    while(ADC_GetResetCalibrationStatus(ADC1));   
   
    // Start ADC1 calibaration    
    ADC_StartCalibration(ADC1);   
    // Check the end of ADC1 calibration    
    while(ADC_GetCalibrationStatus(ADC1));
    // ADC�Զ�У׼����---------------    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ADC����    
     
    prvDMAConfig();
    prvADCNVICInit();
}
void prvDMAConfig()
{
    DMA_InitTypeDef DMA_InitStructure;   
      
    DMA_DeInit(DMA1_Channel1);   
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;   
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;   
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    
    //������ã�ʹ����1�������AD_Value[0]    
    DMA_InitStructure.DMA_BufferSize = 1;   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   
    //ѭ��ģʽ������Bufferд�����Զ��ص���ʼ��ַ��ʼ����    
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);   
    //������ɺ�����DMAͨ��    
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //ʹ��DMA��������ж� 
}


static void prvADCNVICInit(void)
{
	 /*NVIC��ʼ���ṹ��*/
	NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef VECT_TAB_RAM 	   
	 /* Set the Vector Table base location at 0x20000000 */ 	  
	 NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else /* VECT_TAB_FLASH */		  
	 /* Set the Vector Table base location at 0x08000000 */ 	  
	 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	
	#endif	 
	
	 /* ѡ��NVIC���ȷ���4 ����4λ������ռʽ���ȼ�  */
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
//	
	 /*ʹ��DMA1_Channel1_IRQn �ж����� 4��ռ��ʽ���ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADC_DMA_PRIO; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);          // Enable the DMA Interrupt 
}

void GetTemp(u16 advalue)   
{   
    float Vtemp_sensor;
      
//    ADCת�������Ժ󣬶�ȡADC_DR�Ĵ����еĽ����ת���¶�ֵ���㹫ʽ���£�    
//          V25 - VSENSE    
//  T(��) = ------------  + 25    
//           Avg_Slope    
//   V25��  �¶ȴ�������25��ʱ �������ѹ������ֵ1.43 V��    
//  VSENSE���¶ȴ������ĵ�ǰ�����ѹ����ADC_DR �Ĵ����еĽ��ADC_ConvertedValue֮���ת����ϵΪ��    
//            ADC_ConvertedValue * Vdd    
//  VSENSE = --------------------------    
//            Vdd_convert_value(0xFFF)    
//  Avg_Slope���¶ȴ����������ѹ���¶ȵĹ�������������ֵ4.3 mV/�档
//  Current_Temp = (V25 - Vtemp_sensor)/Avg_Slope + 25;     
   
    Vtemp_sensor = advalue * 3.3 / 4096;   
    Current_Temp = (1.43 - Vtemp_sensor)*100/4.3 + 25;   
}    

void DMA1_Channel1_IRQHandler(void)
{
    OSIntEnter();

    if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {
        GetTemp(AD_Value[0]);
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }

    OSIntExit();
}


