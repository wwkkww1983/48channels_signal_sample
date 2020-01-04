#include "timer.h"

void TIMER_init(void)
{
	pwm_Configuration();
	TIM6_init();
	TIM7_init();
}

void pwm_Configuration(void)																//TIM1_CH1 PA8
{
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef         oc;
	
//	DBGMCU_APB1PeriphConfig(DBGMCU_TIM1_STOP,ENABLE);					//使能debug时设断点使定时器停
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);				//TIM1--TIM8使用内部时钟时,由APB2提供

	gpio_AFconfig(GPIOA,GPIO_Pin_8,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_TIM1);
 
	tim.TIM_Prescaler = 1680-1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;									//向上计数
	tim.TIM_Period = 100-1;   																//1000hz
	tim.TIM_ClockDivision = TIM_CKD_DIV1;											//设置时钟分割，与某个滤波器有关这里设啥都没事
	TIM_TimeBaseInit(TIM1,&tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM2;													//选择PWM2则在向上计数时CNT没达到CCRx时输出无效电平		看到下面LOW之后即低电平有效，所以这里输出高
	oc.TIM_OutputState = TIM_OutputState_Enable;							//开启OCx输出到相应引脚
	oc.TIM_Pulse = 0;																					//设置待装入捕获比较器的脉冲值  其实就是这只CCR
	oc.TIM_OCPolarity = TIM_OCPolarity_Low;										//设置输出极性			为低则表示有效电平为低电平
	
	oc.TIM_OutputNState = TIM_OutputNState_Disable;						//失能互补输出	关闭OCN输出到对应引脚				高级定时器使用 比如 TIM_CH1N
	oc.TIM_OCNPolarity = TIM_OCNPolarity_High;								//设置互补输出极性													高级定时器使用
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;								//选择空闲状态下的非工作状态								高级定时器使用
	oc.TIM_OCNIdleState = TIM_OCNIdleState_Set;								//选择互补空闲状态下的非工作状态						高级定时器使用
	
	TIM_OC1Init(TIM1,&oc);																		//通道1
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Disable);					//Enable为下一次更新事件才生效，Disable的话就是立即生效
				 
	TIM_ARRPreloadConfig(TIM1,ENABLE);												//ARR可以重装，这里不是频域的控制所以这个有没有都没有关系		与上面的一个道理
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);													//使能PWM输出
	
	TIM_Cmd(TIM1,ENABLE);
	//		CCRx/ARR 就是占空比		ARR就是通过TIM_Period设置
	TIM1->CCR1 = 50;																					//50%
}

void TIM6_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM6_STOP,ENABLE);														//使能debug时设断点使定时器停
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  												//使能TIM6时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 250 - 1; 														//自动重装载值	即CNT加到500就置零
	TIM_TimeBaseInitStructure.TIM_Prescaler= 8400 - 1;  												//定时器分频		即84M/8400hz使CNT+1	所以这样子计算出来每次进中断的时间为
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 							//向上计数模式												 250/(84M/8400)S		即0.025s 25ms
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);													//初始化TIM6
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); 																		//允许定时器6更新中断  溢出中断
	TIM_Cmd(TIM6,ENABLE); 																											//使能定时器6
	
	nvic_config(TIM6_DAC_IRQn,TIM6_DAC_IRQn_pre,TIM6_DAC_IRQn_sub);
}


void TIM7_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM7_STOP,ENABLE);														//使能debug时设断点使定时器停
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  												//使能TIM7时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 500 - 1; 														//自动重装载值	即CNT加到500就置零
	TIM_TimeBaseInitStructure.TIM_Prescaler= 8400 - 1;  												//定时器分频		即84M/8400hz使CNT+1		所以这样子计算出来每次进中断的时间为
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 							//向上计数模式												  500/(84M/8400)S		即	0.05s 50ms
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);													//初始化TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); 																		//允许定时器7更新中断  溢出中断
	TIM_Cmd(TIM7,ENABLE); 																											//使能定时器7
	
	nvic_config(TIM7_IRQn,TIM7_IRQn_pre,TIM7_IRQn_sub);
}

//以微秒为单位返回系统时间
uint32_t micros(void)
{
	register uint32_t ms, cycle_cnt;
	do {
			ms = sysTickUptime;
			cycle_cnt = SysTick->VAL;
	} while (ms != sysTickUptime);
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

//微秒级延时
void delay_us(uint32_t us)
{
	uint32_t now = micros();
	while (micros() - now < us);
}

//毫秒级延时
void delay_ms(uint32_t ms)
{
	while (ms--)
			delay_us(1000);
}

//以毫秒为单位返回系统时间
uint32_t millis(void)
{
	return sysTickUptime;
}

void TIM6_DAC_IRQHandler(void)																								//定时器6中断服务函数
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) 																//CNT溢出中断
	{
			__nop();																																//只是为了设置断点
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  																//清除中断标志位
}


void TIM7_IRQHandler(void)																										//定时器7中断服务函数
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) 																//CNT溢出中断
	{
			__nop();																																//只是为了设置断点
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  																//清除中断标志位
}
