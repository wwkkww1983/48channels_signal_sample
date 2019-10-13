/*****************************************************************************************************************
 * 	@file 		yixiuge.c
 *  @version 	0.0
 *  @date 		2017.11.25
 *
 *  @brief		这是一个一休哥写的一个主要用于调参跟串口输出的一个文件
 *  					其中使用了STM32F4的串口三
 *						TX		PB10			DMA1_Stream1_ch4
 *  					RX		PB11			DMA1_Stream3_ch4
 *						使用DMA进行串口数据的发送与接收，数据可以是不定长的
 *						发送部分包括一休哥写的数据往数组中推的函数与根据匿名上位机写的函数，最大发送长度由 U3_TXMAX_len 决定
 *						接收部分可接收不定长数据，最大接收长度由  U3_RXMAX_len 决定
 *****************************************************************************************************************/
 
#include "yixiuge.h"

char mode = 0;

void yixiuge_init(void)	
{	
	USATR_config(USART3,115200,GPIOB,GPIO_Pin_10 | GPIO_Pin_11);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);				//开启空闲中断
	
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
	
	DMA_Config(USART3_RX_DMA_Stream,USART_DMA_Channel,(u32)&(USART3->DR),(u32)USART3_RX_buf,U3_RXMAX_len,DMA_DIR_PeripheralToMemory);
	DMA_Cmd(DMA1_Stream1,ENABLE);													//使能接收
	
	DMA_Config(USART3_TX_DMA_Stream,USART_DMA_Channel,(u32)&(USART3->DR),(u32)USART3_TX_buf,U3_TXMAX_len,DMA_DIR_MemoryToPeripheral);
//	DMA_Cmd(DMA1_Stream3,ENABLE);												//使能发送
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET) 	//利用空闲中断判断数据是否通过串口传输完成
	{
		DMA_Cmd(DMA1_Stream1, DISABLE); 										//关闭DMA，防止处理期间有数据
		USART3_RX_updated = 1;																	//置1表示一次接收数据完成
		USART3->SR;  
		USART3->DR;  																						//先取SR再取DR以清除IDLE
		USART3_RX_buf_LENTH =U3_RXMAX_len - DMA1_Stream1->NDTR;				//获得串口发送过来的字符的长度
		memset(USART3_RX_buf+USART3_RX_buf_LENTH,0,DMA1_Stream1->NDTR);		//这样做是为了可以重新覆盖接收数组
		
		DMA_ClearFlag(DMA1_Stream1, DMA_IT_TCIF1);
		DMA1_Stream1->NDTR = U3_RXMAX_len;		  								//重新写入传输量
		DMA_Cmd(DMA1_Stream1, ENABLE);     									//重新打开  
	}  
}

void yixiuge_printf(const char* format, ...)
{
	va_list args;
	int n;
	DMA_Cmd(DMA1_Stream3, DISABLE);                      	//关闭DMA传输 
	va_start(args, format);
	n = vsprintf((char*)USART3_TX_buf, format, args);
	va_end(args);
	
	while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE){}		//确保DMA可以被设置  
	DMA_SetCurrDataCounter(DMA1_Stream3,n);          		  //数据传输量  
	DMA_Cmd(DMA1_Stream3, ENABLE);                      
}

void DMA1_Stream1_IRQHandler(void)											//用于接收的DMA
{
	if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
	{
		DMA_ClearFlag(DMA1_Stream1, DMA_IT_TCIF1);
		DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
	}
}

void DMA1_Stream3_IRQHandler(void)											//用于发送的DMA
{
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3))
	{
		DMA_ClearFlag(DMA1_Stream3, DMA_IT_TCIF3);
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
	}
}



