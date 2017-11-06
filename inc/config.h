#ifndef CONFIG_H
#define CONFIG_H

// The length of the buffer used for processing ADC data
#define ADC_BUFFER_SIZE 512

#define DMA_ADC_TRIGGER_CHANNEL 0
#define DMA_ADC_RESULT_CHANNEL 1
#define DMA_SPI_CHANNEL 2 // Use a non-periodic-capable channel

#define DC_NUM 2
#define RS_NUM 7
#define CS_NUM 4

#endif