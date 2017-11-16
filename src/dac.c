#include "dac.h"
#include "dma.h"
#include "debug.h"
#include "config.h"
#include "MKL26Z4.h"

uint16_t dac_buffer[DAC_BUFFER_SIZE];

static void write_sweep(uint16_t min, uint16_t max)
{
    uint16_t i = 0;

    for(i=0;i<DAC_BUFFER_SIZE;i++)
    {
        dac_buffer[i] = i*(max-min)/DAC_BUFFER_SIZE;
    }
}

void dac_init(void)
{
    // Gate ADC clock
    SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK | SIM_SCGC6_PIT_MASK;
    DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACTRGSEL_MASK | DAC_C0_DACRFS_MASK;
    //DAC0->C1 = DAC_C1_DMAEN_MASK;
    write_sweep(0,(1<<12)-1); // Write a full range sweep to the output buffer

    // Once we start DMA, it doesn't stop
    // It will restart constantly
    dma_conf_dac_out(dac_buffer);
    
    PIT->CHANNEL[1].LDVAL = 4000; //6ksps
    PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TEN_MASK; // Enable PIT timer
}

void dac_write(uint16_t d)
{
    // Write to DATL and DATH with uint8_t types. Does not support 16/32bit writes
    DAC0->DAT[0].DATL = (uint8_t)(d & 0x00FF);
    DAC0->DAT[0].DATH = (uint8_t)((d>>8)&0x000F);
}