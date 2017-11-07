#ifndef ADC_H
#define ADC_H

#include "MKL26Z4.h"
#include "config.h"

int16_t* adc_safe_buffer;

void adc_init(void);

void adc_startread(void);
void adc_restartread(void);

#endif