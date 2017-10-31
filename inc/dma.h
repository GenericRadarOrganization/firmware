#ifndef DMA_H
#define DMA_H

#include "MKL26Z4.h"

void dma_init(void);

void dma_conf_adc_read(uint16_t* dest, uint32_t size);
void dma_conf_adc_trigger(uint32_t sc1_val);
uint32_t dma_in_error(void);
uint32_t dma_transfer_complete(void);
void dma_restart(void);

#endif