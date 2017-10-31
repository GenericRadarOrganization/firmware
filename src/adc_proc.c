#include "MKL26Z4.h"
#include "adc_proc.h"
#include "adc.h"
#include "dma.h"
#include "usb_serial.h"
#include <stdio.h>

void adc_proc_init(void)
{
    adc_startread();
}
void adc_proc_loop(void)
{
    uint32_t size, i;
    char str[128];
    if(dma_transfer_complete()){
        for(i=0;i<10;i++)
        {
            size = snprintf(str,128,"%d ",adc_buffer[i]);
            usb_serial_write(str,size);
        }

        // Here is where we can start playing with the adc buffer
        
        dma_restart();
    }
    if(dma_in_error()){
        usb_serial_write("Fack.\n",sizeof("Fack.\n"));
        dma_restart();
    }
}