#ifndef ADC_PROC_H
#define ADC_PROC_H

#include "MKL26Z4.h"

void adc_proc_init(void);
void adc_proc_loop(void);
uint16_t adc_proc_get_freq(void);

#endif