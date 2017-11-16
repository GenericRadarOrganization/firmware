#include "MKL26Z4.h"
#include "adc_proc.h"
#include "adc.h"
#include "dma.h"
#include "debug.h"
#include "usb_serial.h"
#include <stdio.h>

static float freq;

void adc_proc_init(void)
{
    adc_startread();
}

void adc_proc_loop(void)
{
    uint16_t i;
    float average = 0; // We use a float average here for better accuracy
    uint16_t crossings[10];
    uint16_t crossing_cnt = 0;
    float average_period = 0.0; // The average period is also calculated as a float for accuracy

    // Only process when the DMA buffer is fresh
    if(dma_transfer_complete()){
        // Determine average of input wave
        for(i=0;i<ADC_BUFFER_SIZE;i++)
        {
            average+=((float)adc_safe_buffer[i])/((float)ADC_BUFFER_SIZE);
        }

        // Find zero crossings
        for(i=0;i<ADC_BUFFER_SIZE-1;i++)
        {
            if(crossing_cnt<10){
                if(((float)adc_safe_buffer[i]-average)*((float)adc_safe_buffer[i+1]-average)<0){
                    crossings[crossing_cnt]=i;
                    crossing_cnt++;
                }
            }else{
                // We have enough crossings!
                break;
            }
        }

        // We need at least 3 crossings for this method to work
        if(crossing_cnt>2){
            debug_printf("Crossing count %d\n", crossing_cnt);
            for(i=0;i<(crossing_cnt-2);i++) // Set up such that the last run will have i+2 = crossing_cnt-1
            {
                // Grab period across three crossings
                average_period+=(float)(crossings[i+2]-crossings[i]);
            }
            average_period/=(float)(crossing_cnt-2); // Divide by the number of periods extracted from above

            freq = 6000.0 / average_period; // Frequency found is sample rate over period

            debug_printf("Average freq(Hz): %d\n",(uint16_t)(freq+0.5));
        }
        adc_restartread();
    }
    if(dma_in_error()){
        debug_printf("DMA Broke - Oops.\n");
        adc_restartread();
    }
}

uint16_t adc_proc_get_freq(void)
{
    return (uint16_t)(freq+0.5); // Trick to force float rounding to uint16 (instead of truncation)
}