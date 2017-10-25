#ifndef SYSTICK_H
#define SYSTICK_H

#include "MKL26Z4.h"

void systick_init(void);

uint32_t millis(void);
#endif