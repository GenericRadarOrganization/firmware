#ifndef CONFIG_H
#define CONFIG_H

// The length of the buffer used for processing ADC data
#define ADC_BUFFER_SIZE 1024

#define DAC_BUFFER_SIZE 512

#define DMA_ADC_TRIGGER_CHANNEL 0
#define DMA_ADC_RESULT_CHANNEL 1
#define DMA_SPI_CHANNEL 2 // Use a non-periodic-capable channel

#define DEBUG_PRINT_BUFF_LEN 128

#define DC_NUM 2
#define RS_NUM 7
#define CS_NUM 4


// Quick switch for USB print support enable/disable
// Might be able to save some RAM with it
// Comment it out for SAVINGS!
#define ENABLE_USB_DEBUG

#endif