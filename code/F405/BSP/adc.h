#ifndef __ADC_H
#define __ADC_H	
#include "system.h"

typedef struct
{
	u16 *ADC1_CurrentBuffPtr;
	u16 *ADC2_CurrentBuffPtr;
}ADC_CurrentBuffPtr;

void ADCInit_GPIO(void);
void ADCInit_ADC(void);
void ADCInit_DMA(void);
void ADCInit_Timer(void);
void ADCInit_Nvic(void);
void ADCInit(void);

extern const u8 ADC1_CHANNEL;
extern const u8 ADC2_CHANNEL;

extern bool data_update;
extern u16 *CurrentBuffPtr;

extern bool ADC1_data_update;
extern bool ADC2_data_update;
extern ADC_CurrentBuffPtr ADC_PTR;

extern const u16 ADC_BUFFSIZE;
extern const u8 ADC_CHANNEL;
 							   
//void Adc_Init(void); 				//ADC通道初始化
//u16  Get_Adc(u8 ch); 				//获得某个通道值 
//u16 Get_Adc_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  

#endif 















