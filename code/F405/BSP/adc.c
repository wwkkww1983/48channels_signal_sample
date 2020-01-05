#include "adc.h"

u32 sample_rate = 100000;												//1s采1000个点

/* 数据定义 */
const u16 ADC_BUFFSIZE = 1500;
const u8 ADC_CHANNEL = 3;

u16 uAD_IN_BUFFA[ADC_BUFFSIZE][ADC_CHANNEL];
u16 uAD_IN_BUFFB[ADC_BUFFSIZE][ADC_CHANNEL];

u16 *CurrentBuffPtr = NULL;

void ADCInit_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOA时钟

	//先初始化ADC1通道678 IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PA6 7 通道6 7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;					 //模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			 //不带上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);								 //初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PB0 通道8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不带上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //初始化

}

void ADCInit_ADC(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);	//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); //复位结束

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;										 //独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //两个采样阶段之间的延迟5个时钟(三重模式或双重模式下使用)
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			 //DMA失能(对于多个ADC模式)
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;									 //预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);																		 //初始化

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;												//12位模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;																	//扫描模式(多通道ADC采集要用扫描模式)
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;														//关闭连续转换
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //软件触发
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; //上升沿触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;				//定时器事件2触发ADC
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;												//右对齐
	ADC_InitStructure.ADC_NbrOfConversion = ADC_CHANNEL;													//ADC_CHANNEL个转换在规则序列中
	ADC_Init(ADC1, &ADC_InitStructure);																						//ADC初始化

	//连续模式下,通道的配置
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_15Cycles);	//PA6,通道6,rank=1,表示连续转换中第一个转换的通道,采样时间15个周期(如果数据不稳定可以延时采样时间)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_15Cycles);	//PA7,通道7
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_15Cycles);	//PB0,通道8

	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE); //连续使能DMA
	ADC_DMACmd(ADC1, ENABLE);													//使能ADC_DMA
	ADC_Cmd(ADC1, ENABLE);														//开启AD转换器
}

void ADCInit_DMA(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	//时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	//DMA设置
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;															//选择通道号
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);					//外围设备地址,ADC_DR_DATA规则数据寄存器
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(u16 *)uAD_IN_BUFFA;				//DMA存储器地址,自己设置的缓存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;											//传输方向,外设->存储器
	DMA_InitStructure.DMA_BufferSize = ADC_BUFFSIZE * ADC_CHANNEL;							//DMA缓存大小,数据传输量ADC_BUFFSIZE * ADC_CHANNEL
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//外设是否为增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//存储器是否为增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据长度半个字(16位)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;					//存储器数据长度半字(16位)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;															//DMA模式,普通模式、循环模式,还有双缓冲模式,需要特殊设置
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;													//DMA优先级,高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;											//不使用FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;						//FIFO阈值
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;									//存储器突发,单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;					//外设突发,单次传输
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);																	//初始化DMA2_Stream0,对应为ADC1

	//双缓冲模式设置
	DMA_DoubleBufferModeConfig(DMA2_Stream0, (uint32_t)(u16 *)(uAD_IN_BUFFB), DMA_Memory_0); //DMA_Memory_0首先被传输
	DMA_DoubleBufferModeCmd(DMA2_Stream0, ENABLE);

	//设置DMA中断
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC); //清除中断标志
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);	//传输完成中断
	DMA_Cmd(DMA2_Stream0, ENABLE);									//使能DMA
}

void ADCInit_Timer(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//失能定时器
	TIM_Cmd(TIM2, DISABLE);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); //初始化定时器
	
	u32 Prescaler = 84000000 / 2 / sample_rate;

	//定时器设置,f = 84M / 42000 / 2 = 1KHz  1ms采 1K / 1000 = 1个点
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1;				//分频系数,TIM2时钟在分频系数不为1时,时钟是APB1时钟的两倍84M
	TIM_TimeBaseStructure.TIM_Period = 2 - 1;										//周期值1M/32=31250,CurrentFreq单位为0.1hz,因此需扩大10倍
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//时钟分频因子
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);							//初始化定时器2

	//使能定时器中断
	TIM_ARRPreloadConfig(TIM2, ENABLE);										//允许TIM2定时重载
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update); //选择TIM2的UPDATA事件更新为触发源
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //配置TIM2中断类型

	//使能TIM2
	TIM_Cmd(TIM2, ENABLE);
}

void ADCInit_Nvic(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

//	//定时器中断设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;						//定时器TIM2中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级0
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//子优先级1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化NVIC寄存器

	//DMA中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;		//DMA2_Stream0中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化NVIC寄存器
}

void ADCInit(void)
{
	ADCInit_GPIO();
	ADCInit_DMA();
	ADCInit_Timer();
	ADCInit_ADC();
	ADCInit_Nvic();
}

//void TIM2_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) //判断发生update事件中断
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除update事件中断标志
////		ADC_SoftwareStartConv(ADC1);
//	}
//}

bool data_update = 0;
u32 now = 0,old = 0;
u32 delta_time = 0;
void DMA2_Stream0_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)) //判断DMA传输完成中断
	{
		now = micros();
		delta_time = now - old;
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
		//         DMA_Cmd(DMA2_Stream0, ENABLE); //使能DMA,连续模式下不需要该使能
		//数据转移程序
		if (DMA_GetCurrentMemoryTarget(DMA2_Stream0) == DMA_Memory_0)
		{
			data_update = 1;
			CurrentBuffPtr = uAD_IN_BUFFA[0];
		}
		else
		{
			data_update = 1;
			CurrentBuffPtr = uAD_IN_BUFFB[0];
		}
		old = now;
	}
}

/*
//初始化ADC			PA6 PA7 PB0												   
void  Adc_Init(void)
{    
	GPIO_InitTypeDef			GPIO_InitStructure;
	ADC_CommonInitTypeDef	ADC_CommonInitStructure;
	ADC_InitTypeDef				ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

	//先初始化ADC1通道678 IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//PA6 7 通道6 7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PB0 通道8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化  

 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 3;//3个转换在规则序列中 也就是只转换规则序列3
	ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	

}				  
//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	
			//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_84Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
	
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	ADC_Cmd(ADC1, DISABLE);//开启AD转换器	
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}
*/
