#include "usart.h"

u8 USART3_TX_buf[U3_TXMAX_len] = {0};

u8 USART3_RX_buf[U3_RXMAX_len] = {0};

u8 USART3_RX_updated = 0,USART3_RX_buf_LENTH = 0;

void USATR_config(USART_TypeDef* USARTx,u32 BaudRate,GPIO_TypeDef* GPIOx,u32 Pin)
{
	USART_InitTypeDef USART_InitStructure;
	
	if(USARTx == USART3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		gpio_AFconfig(GPIOx,Pin,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_USART3);
		nvic_config(USART3_IRQn,USART3_IRQn_pre,USART3_IRQn_sub);
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

