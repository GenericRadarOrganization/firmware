#include "adc.h"
#include "MKL26Z4.h"
#include "dma.h"
#include "fixup.h"
#include "config.h"
#include <string.h>

uint16_t adc_buffer[ADC_BUFFER_SIZE];

/* adc_init configures the ADC for continuous hardware triggering and DMA reads */
void adc_init(void){
    // Gate the ADC clock
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK | SIM_SCGC6_PIT_MASK;

    // Configure ADC for DMA triggering, HW triggering, and Alt Vref
    ADC0->SC2 = ADC_SC2_DMAEN_MASK | ADC_SC2_ADTRG_MASK | ADC_SC2_REFSEL(1); // Config for DMA and alt ref

    // Ensure all SC3 options are off
    ADC0->SC3 = 0;

    // Configure for 12bit mode, divide-by-2 clock
    ADC0->CFG1 = ADC_CFG1_MODE(1) | ADC_CFG1_ADIV(1) | ADC_CFG1_ADICLK(0); // ADC clock 24/2MHz
    ADC0->CFG2 = ADC_CFG2_ADHSC_MASK;

    memset((uint16_t*)adc_buffer,10,sizeof(adc_buffer));

    PIT->MCR = 0;
    PIT->CHANNEL[0].LDVAL = 500; //24ksps
    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK; // Enable PIT timer
    SIM->SOPT7 |= SIM_SOPT7_ADC0ALTTRGEN_MASK | SIM_SOPT7_ADC0TRGSEL(4);
}

void adc_startread(void){
    ADC0->SC1[0] = 0x1A;
    dma_conf_adc_read(&adc_buffer[0], ADC_BUFFER_SIZE);
    //dma_conf_adc_trigger(0x1A);
}