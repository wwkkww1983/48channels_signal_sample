#include "system.h"

/**
  * @brief   	参数初始化函数
  * @param 		none
  * @retval   none
  */
void Parameter_Init(void)
{
	
}


/**
  * @brief    系统初始化函数，包含所有外设以及PID结构体的初始化
  * @param 		none
  * @retval   none
  */
void systemInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																			//设置系统中断优先级分组2
	SysTick_Config(SystemCoreClock / 1000);																							//滴答定时器配置，1ms触发一次中断
	yixiuge_init();
	delay_ms(1000);
	TIMER_init();
	ADCInit();
//	LED_init();
	SD_mem_init();
}

/**
  * @brief    采用时间戳方式周期执行相应的任务
  * @param 		none
  * @retval   none
  */
u32 count = 0;
volatile u32 now_time_sys =0, old_time_sys = 0;
volatile u32 delta_time_sys = 0;
void Loop(void)
{
//	SD_save_test();
//	while(1);
//	yixiuge_printf("test\n");
//	delay_ms(10);
	static uint32_t currentTime = 0;
//	static uint32_t loopTime_1ms = 0;
//	static uint32_t loopTime_2ms = 0;
//	static uint32_t loopTime_5ms = 0;
//	static uint32_t loopTime_20ms = 0;
	static uint32_t loopTime_250ms = 0;
//	
	currentTime = micros();																															//获取当前系统时间，单位微秒
	save_adc_data();
	if(count == 200)
	{
//		GPIO_ResetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5);
		yixiuge_printf("test\n");
		delay_ms(10);
		while(1)
		{
		}
	}
//	ch6 = Get_Adc(ADC_Channel_6);
//	ch7 = Get_Adc(ADC_Channel_7);
//	ch8 = Get_Adc(ADC_Channel_8);
//	save_data(ch8);
//	yixiuge_printf("%d\n",ch8);
//	delay_ms(10);
//	if((int32_t)(currentTime - loopTime_1ms) >= 0)  
//	{			
//    loopTime_1ms = currentTime + 1000;																								//1ms控制周期	
//	} 
//	
//	if((int32_t)(currentTime - loopTime_2ms) >= 0)  
//	{			
//    loopTime_2ms = currentTime + 2000;																								//2ms控制周期
//		GPIO_ToggleBits(GPIOA,GPIO_Pin_8);
//		Write_to_Card();
//		yixiuge_SD_handle();
//		send_chassis_angle();
//	}
	
//	if((int32_t)(currentTime - loopTime_5ms) >= 0)  
//	{			
//    loopTime_5ms = currentTime + 5000;																								//5ms控制周期
//	}
//	
//	if((int32_t)(currentTime - loopTime_20ms) >= 0)  
//	{			
//    loopTime_20ms = currentTime + 20000;																							//20ms毫秒控制周期
////		GPIO_ToggleBits(GPIOA,GPIO_Pin_8);
//	}
//	
	if((int32_t)(currentTime - loopTime_250ms) >= 0)  
	{			
    loopTime_250ms = currentTime + 250000;																							//250ms控制周期
//		GPIO_ToggleBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5);
		count++;
//		yixiuge_printf("target_SET X:%d  Y:%d  loctor.YAW:%d  imu.YAW:%.2f\n",loctorData.x,loctorData.y,loctorData.yaw,imu.yaw);
//		yixiuge_printf("target_SET YAW_SET:%.2f  YAW_fb:%.2f\n",gim.pid.yaw_angle_ref,gim.pid.yaw_angle_fdb);
//		yixiuge_printf("target_SET err:%.2f  get:%.2f  set:%.2f\n",pid_yaw.err[NOW],imu.yaw,gim.pid.yaw_angle_ref);
	}
}
