#ifndef __SYSTEM_H
#define	__SYSTEM_H

#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include <stdio.h>
#include "stdbool.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "math.h"
#include "arm_math.h"
#include "my_math.h"

#include "adc.h"

#include "usart.h"
#include "yixiuge.h"
#include "dma.h"
#include "nvic.h"
#include "gpio.h"
#include "timer.h"
#include "sdio_sdcard.h"
#include "malloc.h"

#include "sd.h"

#include "ff.h"  
#include "exfuns.h"    

#define usTicks 168

void Parameter_Init(void);
void systemInit(void);
void Loop(void);

#endif 
