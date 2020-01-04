#include "pwm.h"

void PWM_init(void)
{
	fric_wheel_pwm_Configuration();
//	heat_pwm_init();
}

void fric_wheel_pwm_Configuration(void)																			//TIM4_CH3(PB8)  && TIM4_CH4(PB9)		普通定时器PWM输出
{
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef         oc;
	
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM12_STOP,ENABLE);				//使能debug时设断点使定时器停
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);		  //TIM1--TIM8使用内部时钟时,由APB2提供

	gpio_AFconfig(GPIOH,GPIO_Pin_6 | GPIO_Pin_9,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_TIM12);
 
	tim.TIM_Prescaler = 1800-1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;									//向上计数
	tim.TIM_Period = 1000-1;   																//  90000000/1000/1800 = 50hz
	tim.TIM_ClockDivision = TIM_CKD_DIV1;											//设置时钟分割，与某个滤波器有关这里设啥都没事
	TIM_TimeBaseInit(TIM12,&tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM2;													//选择PWM2则在向上计数时CNT没达到CCRx时输出无效电平		看到下面LOW之后即低电平有效，所以这里输出高
	oc.TIM_OutputState = TIM_OutputState_Enable;							//开启OCx输出到相应引脚
	oc.TIM_Pulse = 0;																					//设置待装入捕获比较器的脉冲值  其实就是这只CCR
	oc.TIM_OCPolarity = TIM_OCPolarity_Low;										//设置输出极性			为低则表示有效电平为低电平
	
	oc.TIM_OutputNState = TIM_OutputNState_Disable;						//失能互补输出	关闭OCN输出到对应引脚				高级定时器使用 比如 TIM_CH1N
	oc.TIM_OCNPolarity = TIM_OCNPolarity_High;								//设置互补输出极性													高级定时器使用
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;								//选择空闲状态下的非工作状态								高级定时器使用
	oc.TIM_OCNIdleState = TIM_OCNIdleState_Set;								//选择互补空闲状态下的非工作状态						高级定时器使用
	
	TIM_OC1Init(TIM12,&oc);																		//通道1
	TIM_OC1PreloadConfig(TIM12,TIM_OCPreload_Enable);					//Enable为下一次更新事件才生效，Disable的话就是立即生效
	TIM_OC2Init(TIM12,&oc);																		//通道2
	TIM_OC2PreloadConfig(TIM12,TIM_OCPreload_Enable);
				 
	TIM_ARRPreloadConfig(TIM12,ENABLE);												//ARR可以重装，这里不是频域的控制所以这个有没有都没有关系		与上面的一个道理
	
	TIM_CtrlPWMOutputs(TIM12,ENABLE);													//使能PWM输出
	
	TIM_Cmd(TIM12,ENABLE);
	//		CCRx/ARR 就是占空比		ARR就是通过TIM_Period设置
	TIM12->CCR1 = 400;
	TIM12->CCR2 = 400;
}

void heat_pwm_init(void)																		//TIM3_CH2(PB5)		普通定时器PWM输出
{
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef         oc;
	
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP,ENABLE);					//使能debug时设断点使定时器停
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		  //TIM1--TIM8使用内部时钟时,由APB2提供

	gpio_AFconfig(GPIOB,GPIO_Pin_5,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_AF_TIM3);
 
	tim.TIM_Prescaler = 84-1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;									//向上计数
	tim.TIM_Period = 1000-1;   																//84000000/1000/84 = 1Khz
	tim.TIM_ClockDivision = TIM_CKD_DIV1;											//设置时钟分割，与某个滤波器有关这里设啥都没事
	TIM_TimeBaseInit(TIM3,&tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM2;													//选择PWM2则在向上计数时CNT没达到CCRx时输出无效电平		看到下面LOW之后即低电平有效，所以这里输出高
	oc.TIM_OutputState = TIM_OutputState_Enable;							//开启OCx输出到相应引脚
	oc.TIM_Pulse = 0;																					//设置待装入捕获比较器的脉冲值  其实就是这只CCR
	oc.TIM_OCPolarity = TIM_OCPolarity_Low;										//设置输出极性			为低则表示有效电平为低电平
	
	oc.TIM_OutputNState = TIM_OutputNState_Disable;						//失能互补输出	关闭OCN输出到对应引脚				高级定时器使用 比如 TIM_CH1N
	oc.TIM_OCNPolarity = TIM_OCNPolarity_High;								//设置互补输出极性													高级定时器使用
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;								//选择空闲状态下的非工作状态								高级定时器使用
	oc.TIM_OCNIdleState = TIM_OCNIdleState_Set;								//选择互补空闲状态下的非工作状态						高级定时器使用
	
	TIM_OC2Init(TIM3,&oc);																		//通道2
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);					//Enable为下一次更新事件才生效，Disable的话就是立即生效
				 
	TIM_ARRPreloadConfig(TIM3,ENABLE);												//ARR可以重装，这里不是频域的控制所以这个有没有都没有关系		与上面的一个道理
	
	TIM_CtrlPWMOutputs(TIM3,ENABLE);													//使能PWM输出
	
	TIM_Cmd(TIM3,ENABLE);
	//		CCRx/ARR 就是占空比		ARR就是通过TIM_Period设置
	TIM3->CCR2 = 0;
}

