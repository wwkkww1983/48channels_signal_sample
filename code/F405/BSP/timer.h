#ifndef __TIMER_H__
#define __TIMER_H__

#include "system.h"

void TIM6_init(void);
void TIM7_init(void);
void TIMER_init(void);

void pwm_Configuration(void);

uint32_t micros(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
uint32_t millis(void);

#endif
