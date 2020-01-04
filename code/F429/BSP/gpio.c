#include "gpio.h"

void gpio_config(GPIO_TypeDef* GPIOx,u32 Pin,GPIOMode_TypeDef Mode,GPIOOType_TypeDef OType,GPIOPuPd_TypeDef PuPd)
{
	GPIO_InitTypeDef  gpio;
	RCC->AHB1ENR |= (1 << ((u32)GPIOx - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE));		//使能时钟
	gpio.GPIO_Pin = Pin;
	gpio.GPIO_Mode = Mode;
  gpio.GPIO_OType = OType;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_PuPd = PuPd;
	GPIO_Init(GPIOx,&gpio);
}

void gpio_AFconfig(GPIO_TypeDef* GPIOx,u32 Pin,GPIOOType_TypeDef OType,GPIOPuPd_TypeDef PuPd,u8 AFto)
{
	u32 pinpos;
	GPIO_InitTypeDef          gpio;
	
	RCC->AHB1ENR |= (1 << ((u32)GPIOx - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE));

	for(pinpos = 0; pinpos < 0x10; pinpos++)
	{
		if((Pin & (1 << pinpos)) == 0)
			continue;
		GPIOx->AFR[pinpos >> 0x03] = (GPIOx->AFR[pinpos >> 0x03] & ~(0x0F << (4 * (pinpos & 0x07)))) | (AFto << (4 * (pinpos & 0x07)));
	}
	gpio.GPIO_Pin = Pin;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_OType = OType;
	gpio.GPIO_PuPd = PuPd;
	GPIO_Init(GPIOx,&gpio);
}

void LED_init(void)
{
	gpio_config(GPIOA,GPIO_Pin_4 | GPIO_Pin_5,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);
	GPIO_SetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5);
}

