#ifndef __GPIO_H
#define __GPIO_H

#include "system.h"

#define LED_RED1_ON			GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define LED_RED2_ON			GPIO_ResetBits(GPIOA,GPIO_Pin_7)

#define LED_RED1_OFF		GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define LED_RED2_OFF		GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define LED_BLUE1_ON		GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define LED_BLUE2_ON		GPIO_ResetBits(GPIOA,GPIO_Pin_6)

#define LED_BLUE1_OFF		GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define LED_BLUE2_OFF		GPIO_SetBits(GPIOA,GPIO_Pin_6)

#define LED_RED_ON			GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_7)
#define LED_RED_OFF			GPIO_SetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_7)
#define LED_BLUE_ON			GPIO_ResetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_6)
#define LED_BLUE_OFF		GPIO_SetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_6)

void gpio_config(GPIO_TypeDef* GPIOx,u32 Pin,GPIOMode_TypeDef Mode,GPIOOType_TypeDef OType,GPIOPuPd_TypeDef PuPd);
void gpio_AFconfig(GPIO_TypeDef* GPIOx,u32 Pin,GPIOOType_TypeDef OType,GPIOPuPd_TypeDef PuPd,u8 AFto);
 							   
void LED_init(void);

#endif 
