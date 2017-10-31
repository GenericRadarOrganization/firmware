#include "adc.h"
#include "MKL26Z4.h"
#include "dma.h"
#include "fixup.h"
#include "config.h"
#include <string.h>

uint16_t adc_buffer[ADC_BUFFER_SIZE];

void adc_init(void){
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    // Gate port 5 clock (not sure if necessary)
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    ADC0->SC2 = ADC_SC2_DMAEN_MASK | ADC_SC2_REFSEL(1); // Config for DMA and alt ref

    ADC0->SC3 = 0;

    ADC0->CFG1 = ADC_CFG1_MODE(1) | ADC_CFG1_ADIV(1) | ADC_CFG1_ADICLK(0); // ADC clock 24/2MHz
    ADC0->CFG2 = ADC_CFG2_ADHSC_MASK;

    memset((uint16_t*)adc_buffer,10,sizeof(adc_buffer));
}

void adc_startread(void){
    dma_conf_adc_read(&adc_buffer[0], ADC_BUFFER_SIZE);
    dma_conf_adc_trigger(0x1A);
}