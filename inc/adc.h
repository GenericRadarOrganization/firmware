#ifndef ADC_H
#define ADC_H

#include "MKL26Z4.h"
#include "config.h"

uint16_t adc_buffer[ADC_BUFFER_SIZE];

void adc_init(void);

void adc_startread(void);

#endif