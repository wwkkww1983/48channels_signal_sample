#ifndef __YIXIUGE_H__
#define __YIXIUGE_H__

#include "system.h"

void yixiuge_init(void);
void exeCmd(void);																				//根据不同的开始字符选择不同的mode
void niming_report(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);
void yixiuge_printf(const char* format, ...);
void yiziuge_tuning(float* Kp,float* Ki,float* Kd,float* target); //调参函数

#endif
