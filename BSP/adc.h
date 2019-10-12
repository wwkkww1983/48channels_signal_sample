#ifndef __ADC_H
#define __ADC_H	
#include "system.h"

void ADCInit_GPIO(void);
void ADCInit_ADC(void);
void ADCInit_DMA(void);
void ADCInit_Timer(void);
void ADCInit_Nvic(void);
void ADCInit(void);

extern bool data_update;
extern u16 *CurrentBuffPtr;
 							   
//void Adc_Init(void); 				//ADC通道初始化
//u16  Get_Adc(u8 ch); 				//获得某个通道值 
//u16 Get_Adc_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  

#endif 















