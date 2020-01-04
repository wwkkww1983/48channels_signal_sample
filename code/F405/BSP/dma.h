#ifndef __DMA_H
#define __DMA_H	

#include "system.h"

#define USART1_RX_DMA_Stream		DMA2_Stream5
#define USART2_RX_DMA_Stream		DMA1_Stream5
#define USART3_RX_DMA_Stream		DMA1_Stream1
#define UART4_RX_DMA_Stream			DMA1_Stream2
#define UART5_RX_DMA_Stream			DMA1_Stream0

#define USART1_TX_DMA_Stream		DMA2_Stream7
#define USART2_TX_DMA_Stream		DMA1_Stream6
#define USART3_TX_DMA_Stream		DMA1_Stream3
#define UART4_TX_DMA_Stream			DMA1_Stream4
#define UART5_TX_DMA_Stream			DMA1_Stream7

#define USART_DMA_Channel				DMA_Channel_4

void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 PeripheralBaseAddr,u32 MemoryBaseAddr,u16 buf_len,u32 direction);
					   
#endif 

