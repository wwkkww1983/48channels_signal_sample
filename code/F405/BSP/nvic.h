#ifndef __NVIC_H
#define __NVIC_H	
#include "system.h"

/*************************PreemptionPriority*************************/
#define DMA2_Stream5_IRQn_pre				0						//遥控器接收
#define CAN1_RX0_IRQn_pre						0
#define CAN2_RX0_IRQn_pre						0
#define USART3_IRQn_pre							0						//串口三空闲中断

#define DMA1_Stream3_IRQn_pre				1						//串口三TX
#define UART4_IRQn_pre							1						//串口四空闲中断
#define DMA1_Stream4_IRQn_pre				1						//串口四TX
#define UART5_IRQn_pre							1						//串口五空闲中断

#define TIM1_CC_IRQn_pre						2
#define TIM6_DAC_IRQn_pre						2
#define TIM7_IRQn_pre								2

/*****************************SubPriority*****************************/
#define DMA2_Stream5_IRQn_sub				0						//遥控器接收
#define CAN1_RX0_IRQn_sub						1
#define CAN2_RX0_IRQn_sub						2
#define USART3_IRQn_sub							3						//串口三空闲中断

#define DMA1_Stream3_IRQn_sub				0						//串口三TX
#define UART4_IRQn_sub							1						//串口四空闲中断
#define DMA1_Stream4_IRQn_sub				2						//串口四TX
#define UART5_IRQn_sub							3						//串口五空闲中断

#define TIM1_CC_IRQn_sub						0
#define TIM6_DAC_IRQn_sub						1
#define TIM7_IRQn_sub								2
 							   
void nvic_config(u8 IRQChannel,u8 PreemptionPriority,u8 SubPriority);
								 
#endif 

