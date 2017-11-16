#ifndef DAC_H
#define DAC_H

#include "MKL26Z4.h"

void dac_init(void);
void dac_write(uint16_t d);

#endif