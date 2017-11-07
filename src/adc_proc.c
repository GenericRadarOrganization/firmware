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
    int16_t average = 0;
    uint16_t crossings[10];
    uint16_t crossing_cnt = 0;
    uint16_t average_period = 0;
    if(dma_transfer_complete()){
        
        // Determine average of input wave
        for(i=0;i<ADC_BUFFER_SIZE;i++)
        {
            average+=adc_safe_buffer[i]/ADC_BUFFER_SIZE;
        }
        //debug_printf("Average: %i\n", average);

        // Subtract the average from every value
        for(i=0;i<ADC_BUFFER_SIZE;i++)
        {
            adc_safe_buffer[i]-=average;
        }

        // Find zero crossings
        for(i=0;i<ADC_BUFFER_SIZE-1;i++)
        {
            if(crossing_cnt<10){
                if(adc_safe_buffer[i]*adc_safe_buffer[i+1]<0){
                    crossings[crossing_cnt]=i;
                    crossing_cnt++;
                }
            }
        }

        // Find average period from crossings
        if(crossing_cnt>0){
            for(i=0;i<crossing_cnt-1;i++)
            {
                average_period+=(crossings[i+1]-crossings[i])/(crossing_cnt-1);
            }
        }
        
        //debug_printf("Average period: %d\n",average_period);
        adc_restartread();
    }
    if(dma_in_error()){
        debug_printf("Fack.\n");
        adc_restartread();
    }
}