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
    uint32_t i;
    float average = 0;
    uint16_t crossings[10];
    uint16_t crossing_cnt = 0;
    float average_period = 0.0;
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
            }
        }

        // Find average period from crossings (technically the half-period)
        if(crossing_cnt>2){ // Need at least three crossings to work accurately
            debug_printf("Crossing count %d\n", crossing_cnt);
            for(i=0;i<(crossing_cnt-2);i++) // One execution for three crossings, 8 for 10
            {
                //debug_printf("run\n",crossings[i]);
                average_period+=(float)(crossings[i+2]-crossings[i]);
            }
            average_period/=(float)(crossing_cnt-2); // Divide by the number of periods exracted from above
            //average_period-=1.0f;
            //debug_printf("%d\n",crossings[i]);

            //debug_printf("Average period: %.2f\n",average_period);

            freq = 6000.0 / average_period;

            debug_printf("Average freq(Hz): %d\n",(uint16_t)(freq+0.5));
        }
        adc_restartread();
    }
    if(dma_in_error()){
        debug_printf("Fack.\n");
        adc_restartread();
    }
}

uint16_t adc_proc_get_freq(void)
{
    return (uint16_t)(freq+0.5);
}