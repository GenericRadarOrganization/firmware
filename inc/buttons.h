#ifndef BUTTONS_H
#define BUTTONS_H

#include "MKL26Z4.h"

#define BUTTON_1_MASK (1<<0)
#define BUTTON_2_MASK (1<<1)
#define BUTTON_3_MASK (1<<2)
#define BUTTON_4_MASK (1<<3)
#define BUTTON_MASK 0x0F

void button_init(void);
void button_loop(void);

uint8_t button_state(void);
void button_ack(uint8_t mask);

#endif