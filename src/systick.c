
#include "MKL26Z4.h"

#define PIT_TIMER_PERIOD 0xBB80-1; // ie 48000 (Should theoretically give a 1000Hz interrupt)

uint32_t ticks;

void systick_init(void){
    ticks = 0;
    // Enable PIT Clock
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

    // Ensure MDIS is low (Enable PIT Module)
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;

    PIT->CHANNEL[0].LDVAL = PIT_TIMER_PERIOD;

    PIT->CHANNEL[0].TFLG;

    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;

    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);
}

uint32_t millis(void){
    return ticks;
}
void pit_isr(void){
    //Check if interrupt caused by this timer
    if(PIT->CHANNEL[0].TFLG&PIT_TFLG_TIF_MASK){
        PIT->CHANNEL[0].TFLG|=PIT_TFLG_TIF_MASK; // Clear interrupt
        PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
        PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
    }
}