#ifndef __SD_H__
#define __SD_H__

#include "system.h"

void SD_mem_init(void);

void Write_to_Card(void);

void save_data(u16 data);

void save_adc_data(void);

void SD_save_test(void);

void text_new_pathname(u8 *pname);

void save_big_data_test(void);

#endif
