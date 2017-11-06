#include "MKL26Z4.h"
#include "adc_proc.h"
#include "adc.h"
#include "dma.h"
#include "debug.h"
#include "usb_serial.h"
#include <stdio.h>

void adc_proc_init(void)
{
    adc_startread();
}

void adc_proc_loop(void)
{
    uint32_t i;
    if(dma_transfer_complete()){
        for(i=0;i<10;i++)
        {
            debug_printf("%d ",adc_buffer[i]);
        }
        debug_printf("\n");
        // Here is where we can start playing with the adc buffer
        dma_restart();
    }
    if(dma_in_error()){
        debug_printf("Fack.\n");
        dma_restart();
    }
}