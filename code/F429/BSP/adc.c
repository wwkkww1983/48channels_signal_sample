#include "adc.h"

u32 sample_rate = 1000;												//1s采1000个点

/* 数据定义 */
const u16 ADC_BUFFSIZE = 1000;
const u8 ADC_CHANNEL = 24;
const u8 ADC1_CHANNEL = 16;
const u8 ADC3_CHANNEL = 8;

u16 test_ADC1_BUFFA[2][2]={{2,3},{1,1}};
u16 test_ADC1_BUFFB[2][2]={{2,2},{1,1}};
u16 test_ADC3_BUFFA[2][2]={{15,15},{15,15}};
u16 test_ADC3_BUFFB[2][2]={{15,15},{15,15}};

u16 ADC1_BUFFA[ADC_BUFFSIZE][ADC1_CHANNEL];
u16 ADC1_BUFFB[ADC_BUFFSIZE][ADC1_CHANNEL];
u16 ADC3_BUFFA[ADC_BUFFSIZE][ADC3_CHANNEL];
u16 ADC3_BUFFB[ADC_BUFFSIZE][ADC3_CHANNEL];

ADC_CurrentBuffPtr ADC_PTR = {NULL,NULL};

void ADCInit_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); //使能GPIOF时钟

	/* ADC1的16个通道 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //PA01234567 对应通道ADC1的01234567
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;					 //模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			 //不带上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);								 //初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;				 //PB01 ADC1通道89
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不带上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5;				 //PC012345 对应通道ADC1的10 11 12 13 14 15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不带上下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //初始化
	
	/* ADC3的8个通道 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 |GPIO_Pin_9 |GPIO_Pin_10;				 //PF345678910 对应通道ADC3的9 14 15 4 5 6 7 8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不带上下拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);					 //初始化
}

void ADCInit_ADC(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE); //使能ADC3时钟

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);	//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); //复位结束
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3, ENABLE);	//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3, DISABLE); //复位结束

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
	ADC_InitStructure.ADC_NbrOfConversion = ADC1_CHANNEL;													//ADC1有16个转换在规则序列中
	ADC_Init(ADC1, &ADC_InitStructure);																						//ADC1初始化
	ADC_InitStructure.ADC_NbrOfConversion = ADC3_CHANNEL;													//ADC3有8个转换在规则序列中
	ADC_Init(ADC3, &ADC_InitStructure);																						//ADC初始化


	//连续模式下,通道的配置
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_15Cycles);		//PA0,通道0,rank=1,表示连续转换中第一个转换的通道,采样时间15个周期(如果数据不稳定可以延时采样时间)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_15Cycles);		//PA1,通道1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_15Cycles);		//PA2,通道2
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_15Cycles);		//PA3,通道3
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_15Cycles);		//PA4,通道4
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_15Cycles);		//PA5,通道5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_15Cycles);		//PA6,通道6
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_15Cycles);		//PA7,通道7
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_15Cycles);		//PB0,通道8
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 10, ADC_SampleTime_15Cycles);		//PB1,通道9
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 11, ADC_SampleTime_15Cycles);	//PC0,通道10
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 12, ADC_SampleTime_15Cycles);	//PC1,通道11
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 13, ADC_SampleTime_15Cycles);	//PC2,通道12
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 14, ADC_SampleTime_15Cycles);	//PC3,通道13
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 15, ADC_SampleTime_15Cycles);	//PC4,通道14
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 16, ADC_SampleTime_15Cycles);	//PC5,通道15
	
	ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 1, ADC_SampleTime_15Cycles);		//PF6,通道4,rank=1,表示连续转换中第一个转换的通道,采样时间15个周期(如果数据不稳定可以延时采样时间)
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 2, ADC_SampleTime_15Cycles);		//PF7,通道5
	ADC_RegularChannelConfig(ADC3, ADC_Channel_6, 3, ADC_SampleTime_15Cycles);		//PF8,通道6
	ADC_RegularChannelConfig(ADC3, ADC_Channel_7, 4, ADC_SampleTime_15Cycles);		//PF9,通道7
	ADC_RegularChannelConfig(ADC3, ADC_Channel_8, 5, ADC_SampleTime_15Cycles);		//PF10,通道8
	ADC_RegularChannelConfig(ADC3, ADC_Channel_9, 6, ADC_SampleTime_15Cycles);		//PF3,通道9
	ADC_RegularChannelConfig(ADC3, ADC_Channel_14, 7, ADC_SampleTime_15Cycles);		//PF14,通道14
	ADC_RegularChannelConfig(ADC3, ADC_Channel_15, 8, ADC_SampleTime_15Cycles);		//PF16,通道15

	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE); //连续使能DMA
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE); //连续使能DMA
	ADC_DMACmd(ADC3, ENABLE);													//使能ADC_DMA
	ADC_DMACmd(ADC1, ENABLE);													//使能ADC_DMA
	ADC_Cmd(ADC1, ENABLE);														//开启AD转换器
	ADC_Cmd(ADC3, ENABLE);														//开启AD转换器
}

void ADCInit_DMA(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	//时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);												//ADC1用通道0数据流0 ADC3用通道2数据流1

	//AD1的DMA设置
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;															//选择通道号 ADC1用通道0数据流0
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);					//外围设备地址,ADC_DR_DATA规则数据寄存器
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(u16 *)ADC1_BUFFA;				//DMA存储器地址,自己设置的缓存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;											//传输方向,外设->存储器
	DMA_InitStructure.DMA_BufferSize = ADC_BUFFSIZE * ADC1_CHANNEL;							//DMA缓存大小,数据传输量ADC_BUFFSIZE * ADC_CHANNEL
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
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_2;															//选择通道号 ADC3用通道2数据流1
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC3->DR);					//外围设备地址,ADC_DR_DATA规则数据寄存器
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(u16 *)ADC3_BUFFA;				//DMA存储器地址,自己设置的缓存地址
	DMA_InitStructure.DMA_BufferSize = ADC_BUFFSIZE * ADC3_CHANNEL;							//DMA缓存大小,数据传输量ADC_BUFFSIZE * ADC_CHANNEL
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);																	//初始化DMA2_Stream1,对应为ADC3

	//双缓冲模式设置
	DMA_DoubleBufferModeConfig(DMA2_Stream0, (uint32_t)(u16 *)(ADC1_BUFFB), DMA_Memory_0); //DMA_Memory_0首先被传输
	DMA_DoubleBufferModeCmd(DMA2_Stream0, ENABLE);
	DMA_DoubleBufferModeConfig(DMA2_Stream1, (uint32_t)(u16 *)(ADC3_BUFFB), DMA_Memory_0); //DMA_Memory_0首先被传输
	DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);

	//设置DMA中断
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC); //清除中断标志
	DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TC); //清除中断标志
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);	//传输完成中断
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);	//传输完成中断
	DMA_Cmd(DMA2_Stream0, ENABLE);									//使能DMA
	DMA_Cmd(DMA2_Stream1, ENABLE);									//使能DMA
}

void ADCInit_Timer(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//失能定时器
	TIM_Cmd(TIM2, DISABLE);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); //初始化定时器
	
	u32 Prescaler = 90000000 / 2 / sample_rate;

	//定时器设置,f = 90M / 45000 / 2 = 1KHz  1ms采 1K / 1000 = 1个点
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1;				//分频系数,TIM2时钟在分频系数不为1时,时钟是APB1时钟的两倍90M
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化NVIC寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;		//DMA2_Stream1中断
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

bool ADC1_data_update = 0;
bool ADC3_data_update = 0;
volatile u32 now = 0,old = 0;
volatile u32 delta_time = 0;
void DMA2_Stream0_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)) //判断DMA传输完成中断
	{
//		now = micros();
//		delta_time = now - old;
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
		//数据转移程序
		if (DMA_GetCurrentMemoryTarget(DMA2_Stream0) == DMA_Memory_0)
		{
			ADC1_data_update = 1;
//			CurrentBuffPtr = ADC1_BUFFA[0];
			ADC_PTR.ADC1_CurrentBuffPtr = test_ADC1_BUFFA[0];
		}
		else
		{
			ADC1_data_update = 1;
//			CurrentBuffPtr = ADC1_BUFFB[0];
			ADC_PTR.ADC1_CurrentBuffPtr = test_ADC1_BUFFB[0];
		}
//		old = now;
	}
}
void DMA2_Stream1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1)) //判断DMA传输完成中断
	{
		now = micros();
		delta_time = now - old;
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
		//数据转移程序
		if (DMA_GetCurrentMemoryTarget(DMA2_Stream1) == DMA_Memory_0)
		{
			ADC3_data_update = 1;
			ADC_PTR.ADC3_CurrentBuffPtr = test_ADC3_BUFFA[0];
		}
		else
		{
			ADC3_data_update = 1;
			ADC_PTR.ADC3_CurrentBuffPtr = test_ADC3_BUFFB[0];
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
