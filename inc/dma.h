#ifndef DMA_H
#define DMA_H

#include "MKL26Z4.h"

void dma_init(void);

void dma_conf_spi();
void dma_conf_adc_read(int16_t* dest, uint32_t size);
void dma_conf_dac_out(uint16_t* src);
void dma_spi_tx(void* src, uint32_t size);
void dma_spi_tx_wait(void* src, uint32_t size);
uint32_t dma_in_error(void);
uint32_t dma_transfer_complete(void);
void dma_restart(int16_t* dest);

#endif