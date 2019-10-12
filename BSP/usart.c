#include "usart.h"

u8 USART1_TX_buf[U1_TXMAX_len] = {0};
u8 USART2_TX_buf[U2_TXMAX_len] = {0};
u8 USART3_TX_buf[U3_TXMAX_len] = {0};
u8 UART4_TX_buf[U4_TXMAX_len]  = {0};
u8 UART5_TX_buf[U5_TXMAX_len]  = {0};

u8 USART1_RX_buf[U1_RXMAX_len] = {0};
u8 USART2_RX_buf[U2_RXMAX_len] = {0};
u8 USART3_RX_buf[U3_RXMAX_len] = {0};
u8 UART4_RX_buf[U4_RXMAX_len]  = {0};
u8 UART5_RX_buf[U5_RXMAX_len]  = {0};

u8 USART1_RX_updated = 0,USART1_RX_buf_LENTH = 0;
u8 USART2_RX_updated = 0,USART2_RX_buf_LENTH = 0;
u8 USART3_RX_updated = 0,USART3_RX_buf_LENTH = 0;
u8 UART4_RX_updated  = 0,UART4_RX_buf_LENTH  = 0;
u8 UART5_RX_updated  = 0,UART5_RX_buf_LENTH  = 0;

void USATR_config(USART_TypeDef* USARTx,u32 BaudRate,GPIO_TypeDef* GPIOx,u32 Pin)
{
	USART_InitTypeDef USART_InitStructure;
	
	if(USARTx == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
		gpio_AFconfig(GPIOx,Pin,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_USART1);
//		nvic_config(USART1_IRQn,USART1_IRQn_pre,USART1_IRQn_sub);
	}
	if(USARTx == USART2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		gpio_AFconfig(GPIOx,Pin,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_USART2);
//		nvic_config(USART2_IRQn,USART2_IRQn_pre,USART2_IRQn_sub);
	}	
	if(USARTx == USART3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		gpio_AFconfig(GPIOx,Pin,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_USART3);
		nvic_config(USART3_IRQn,USART3_IRQn_pre,USART3_IRQn_sub);
	}
	if(USARTx == UART4)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
		gpio_AFconfig(GPIOx,Pin,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_UART4);
		nvic_config(UART4_IRQn,UART4_IRQn_pre,UART4_IRQn_sub);
	}
	if(USARTx == UART5)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
		gpio_AFconfig(GPIOC,GPIO_Pin_12,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_UART5);
		gpio_AFconfig(GPIOD,GPIO_Pin_2,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_UART5);
		nvic_config(UART5_IRQn,UART5_IRQn_pre,UART5_IRQn_sub);
	}

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USARTx,&USART_InitStructure);
	
	USART_Cmd(USARTx,ENABLE);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) != SET);
}

void UART4_Configuration(void)
{
	USATR_config(UART4,115200,GPIOA,GPIO_Pin_0 | GPIO_Pin_1);
	
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);				//开启空闲中断
	
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);
	
	DMA_Config(UART4_RX_DMA_Stream,USART_DMA_Channel,(u32)&(UART4->DR),(u32)UART4_RX_buf,U4_RXMAX_len,DMA_DIR_PeripheralToMemory);
	DMA_Cmd(DMA1_Stream2,ENABLE);													//使能接收
	
	DMA_Config(UART4_TX_DMA_Stream,USART_DMA_Channel,(u32)&(UART4->DR),(u32)UART4_TX_buf,U4_TXMAX_len,DMA_DIR_MemoryToPeripheral);
//	DMA_Cmd(DMA1_Stream4,ENABLE);												//使能发送
}

void UART5_Configuration(void)
{
	USATR_config(UART5,115200,GPIOC,GPIO_Pin_12 | GPIO_Pin_2);
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);				//开启空闲中断
	
	USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);
	DMA_Config(UART5_RX_DMA_Stream,USART_DMA_Channel,(u32)&(UART5->DR),(u32)UART5_RX_buf,U5_RXMAX_len,DMA_DIR_PeripheralToMemory);
	DMA_Cmd(DMA1_Stream0,ENABLE);													//使能接收
}

void UART4_printf(const char* format, ...)
{
	va_list args;
	int n;
	DMA_Cmd(DMA1_Stream4, DISABLE);                      	//关闭DMA传输
	va_start(args, format);
	n = vsprintf((char*)UART4_TX_buf, format, args);
	va_end(args);
	
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}		//确保DMA可以被配置
	DMA_SetCurrDataCounter(DMA1_Stream4,n);          		  //数据传输量
	DMA_Cmd(DMA1_Stream4, ENABLE);                      
}

void usart_init(void)
{
	UART5_Configuration();
	UART4_Configuration();
}

void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4,USART_IT_IDLE) != RESET) 	//利用空闲中断判断数据是否通过串口传输完成
	{
		DMA_Cmd(DMA1_Stream2, DISABLE); 										//关闭DMA，防止处理数据期间有数据
		UART4_RX_updated = 1;																	//置1表示一次接收数据完成
		UART4->SR;  
		UART4->DR;  												//先取SR再取DR以清除IDLE
		UART4_RX_buf_LENTH =U4_RXMAX_len - DMA1_Stream2->NDTR;				//获得串口发送过来的字符的长度
		memset(UART4_RX_buf+UART4_RX_buf_LENTH,0,DMA1_Stream2->NDTR);		//这样做是为了可以重新覆盖接收数组
		
		DMA_ClearFlag(DMA1_Stream2, DMA_IT_TCIF2);
		DMA1_Stream2->NDTR = U4_RXMAX_len;		  								//重新写入传输量
		DMA_Cmd(DMA1_Stream2, ENABLE);     									//重新打开
	}  
}

void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5,USART_IT_IDLE) != RESET) 							//利用空闲中断判断数据是否通过串口传输完成
	{
		DMA_Cmd(DMA1_Stream0, DISABLE); 																//关闭DMA，防止处理数据期间有数据
		UART5_RX_updated = 1;																						//置1表示一次接收数据完成
		UART5->SR;  
		UART5->DR;  																										//先取SR再取DR以清除IDLE
		UART5_RX_buf_LENTH =U5_RXMAX_len - DMA1_Stream0->NDTR;					//获得串口发送过来的字符的长度
		memset(UART5_RX_buf+UART5_RX_buf_LENTH,0,DMA1_Stream0->NDTR);		//这样做是为了可以重新覆盖接收数组
		
		DMA_ClearFlag(DMA1_Stream0, DMA_IT_TCIF0);
		DMA1_Stream0->NDTR = U5_RXMAX_len;		  												//重新写入传输量
		DMA_Cmd(DMA1_Stream0, ENABLE);     															//重新打开
	}  
}

void DMA1_Stream4_IRQHandler(void)											//用于UART4发送的DMA中断
{
	if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4))
	{
		DMA_ClearFlag(DMA1_Stream4, DMA_IT_TCIF4);
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
	}
}

