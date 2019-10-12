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
bool bufStartsWith(char *st)														//判断数组以何字符开始
{
	char i;
  for(i = 0; ; i++)
  {
    if (st[i] == 0) return true;
    if (USART3_RX_buf[i] == 0) return false;
    if (USART3_RX_buf[i] != st[i]) return false;;
  }
}

void exeCmd(void)																				//根据不同的开始字符选择不同的mode
{
  if ( bufStartsWith("P ") )      mode = 1;
  if ( bufStartsWith("I ") )      mode = 2;
  if ( bufStartsWith("D ") )      mode = 3;
  if ( bufStartsWith("t+5") )    	mode = 4;
  if ( bufStartsWith("t-5") )    	mode = 5;
  if ( bufStartsWith("target ") ) mode = 6;
}

int fputc(int ch, FILE *f)															//重定义printf估计以后都不会用到了
{
	USART_SendData(USART3, (unsigned char) ch);
	while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);	
	return (ch);
}

//传送数据给匿名地面站
void niming_report(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 i;
	DMA_Cmd(DMA1_Stream3, DISABLE);                      //关闭DMA传输 
//	memset(USART3_TX_buf,0,23);
	USART3_TX_buf[0]=0XAA;															 				 //帧头
	USART3_TX_buf[1]=0XAA;															 				 //帧头
	USART3_TX_buf[2]=0XF1;															 				 //功能字
	USART3_TX_buf[3]=18;																 				 //数据长度
	
	USART3_TX_buf[4]=(aacx>>8)&0XFF;
	USART3_TX_buf[5]=aacx&0XFF;
	USART3_TX_buf[6]=(aacy>>8)&0XFF;
	USART3_TX_buf[7]=aacy&0XFF;
	USART3_TX_buf[8]=(aacz>>8)&0XFF;
	USART3_TX_buf[9]=aacz&0XFF; 
	USART3_TX_buf[10]=(gyrox>>8)&0XFF;
	USART3_TX_buf[11]=gyrox&0XFF;
	USART3_TX_buf[12]=(gyroy>>8)&0XFF;
	USART3_TX_buf[13]=gyroy&0XFF;
	USART3_TX_buf[14]=(gyroz>>8)&0XFF;
	USART3_TX_buf[15]=gyroz&0XFF;	
	USART3_TX_buf[16]=(roll>>8)&0XFF;
	USART3_TX_buf[17]=roll&0XFF;
	USART3_TX_buf[18]=(pitch>>8)&0XFF;
	USART3_TX_buf[19]=pitch&0XFF;
	USART3_TX_buf[20]=(yaw>>8)&0XFF;
	USART3_TX_buf[21]=yaw&0XFF;
	USART3_TX_buf[22]=0;																					//校验数置零
	for(i=0;i<22;i++)USART3_TX_buf[22]+=USART3_TX_buf[i];								//计算校验和	
	
	while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE){}		//确保DMA可以被设置  
	DMA_SetCurrDataCounter(DMA1_Stream3,23);          		//数据传输量  
	DMA_Cmd(DMA1_Stream3, ENABLE);                      	//开启DMA传输 
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

void yiziuge_tuning(float* Kp,float* Ki,float* Kd,float* target) //调参函数
{
	if(USART3_RX_updated)
	{
		USART3_RX_updated = 0;
		exeCmd();
	}
	switch(mode)																					//根据mode的不同调节不同的参数
	{
		case 1: mode = 0;
			*Kp = atof((char*)USART3_RX_buf + 2);
			yixiuge_printf("P_SET P set to %.2f\n",*Kp);
			break;
		
		case 2: mode = 0;
			*Ki = atof((char*)USART3_RX_buf + 2);
			yixiuge_printf("I_SET I set to %.2f\n",*Ki);
			break;
		
		case 3: mode = 0;
			*Kd = atof((char*)USART3_RX_buf + 2);
			yixiuge_printf("D_SET D set to %.2f\n",*Kd);
			break;
		
		case 4: mode = 0;
			*target = *target + atof((char*)USART3_RX_buf + 2);
			yixiuge_printf("target_SET target set to %.2f\n",*target);
			break;
		
		case 5: mode = 0;
			*target = *target - atof((char*)USART3_RX_buf + 2);
			yixiuge_printf("target_SET target set to %.2f\n",*target);
			break;
		
		case 6: mode = 0;
			*target = atof((char*)USART3_RX_buf + 7);
			yixiuge_printf("target_SET target set to %.2f\n",*target);
			break;
			
		default:
			break;
	}
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



