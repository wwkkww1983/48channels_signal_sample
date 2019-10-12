/********************************************************************
             |PINSPACK 1     |PINSPACK 2     |PINSPACK 3	
U(S)ARTX     |TX     RX      |TX     RX      |TX     RX

USART1       |PA9    PA10    |PB6    PB7     |-      -
USART2       |PA2    PA3     |PD5    PD6     |-      -
USART3       |PB10   PB11    |PC10   PC11    |PD8    PD9
UART4        |PA0    PA1     |PC10   PC11    |-      -
UART5        |PC12   PD2     |-      -       |-      -
USART6       |PC6    PC7     |PG14   PG9     |-      -
*********************************************************************/
#ifndef __USART_H__
#define __USART_H__

#include "system.h"

#define U1_TXMAX_len 32
#define U1_RXMAX_len 32

#define U2_TXMAX_len 32
#define U2_RXMAX_len 32

#define U3_TXMAX_len 128
#define U3_RXMAX_len 128

#define U4_TXMAX_len 32
#define U4_RXMAX_len 128

#define U5_TXMAX_len 32
#define U5_RXMAX_len 128

extern u8 USART1_TX_buf[U1_TXMAX_len];
extern u8 USART2_TX_buf[U2_TXMAX_len];
extern u8 USART3_TX_buf[U3_TXMAX_len];
extern u8 UART4_TX_buf[U4_TXMAX_len];
extern u8 UART5_TX_buf[U5_TXMAX_len];

extern u8 USART1_RX_buf[U1_RXMAX_len];
extern u8 USART2_RX_buf[U2_RXMAX_len];
extern u8 USART3_RX_buf[U3_RXMAX_len];
extern u8 UART4_RX_buf[U4_RXMAX_len];
extern u8 UART5_RX_buf[U5_RXMAX_len];

extern u8 USART1_RX_updated,USART1_RX_buf_LENTH;
extern u8 USART2_RX_updated,USART2_RX_buf_LENTH;
extern u8 USART3_RX_updated,USART3_RX_buf_LENTH;
extern u8 UART4_RX_updated ,UART4_RX_buf_LENTH;
extern u8 UART5_RX_updated ,UART5_RX_buf_LENTH;

void USATR_config(USART_TypeDef* USARTx,u32 BaudRate,GPIO_TypeDef* GPIOx,u32 Pin);
void usart_init(void);

void UART4_printf(const char* format, ...);
	
#endif
