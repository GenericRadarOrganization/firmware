#include "buttons.h"
#include "MKL26Z4.h"
#include <string.h>

#define HISTORY_LEN 8

uint8_t history[HISTORY_LEN];

uint8_t button_flags;

void button_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[0] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTB->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTB->PCR[2] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTB->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    memset(history,0,sizeof(history));
    button_flags = 0;
}

void button_loop(void)
{
    int i;
    uint8_t temp = 0x0F;
    // i = 9, 8, 7, 6, 5, 4, 3, 2, 1
    for(i=HISTORY_LEN-1;i>0;i--){
        history[i] = history[i-1]; // Shift the array right 1
    }
    history[0] = (uint8_t)(~(PTB->PDIR & 0x0000000F));
    for(i=0;i<HISTORY_LEN;i++){
        temp&=history[i];
    }
    button_flags |= temp;
}

uint8_t button_state(void)
{
    return button_flags;
}
void button_ack(uint8_t mask)
{
    button_flags &= ~(mask);
}