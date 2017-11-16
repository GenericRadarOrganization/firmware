#include "fixup.h"
#include "systick.h"
#include "tsk.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "lcd.h"
#include "config.h"

#include "usb_dev.h"
#include "usb_serial.h"

#define LED  (1U << 5)

int main(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK;

    PORTD->PCR[1] = PORT_PCR_MUX(2U); //sck
    PORTC->PCR[6] = PORT_PCR_MUX(2U); // MOSI
    PORTC->PCR[7] = PORT_PCR_MUX(2U); // MISO

    PORTD->PCR[CS_NUM] = PORT_PCR_MUX(1U) | PORT_PCR_DSE_MASK; //cs
    PORTD->PCR[DC_NUM] = PORT_PCR_MUX(1U) | PORT_PCR_DSE_MASK; // Enable DC pin
    PORTD->PCR[RS_NUM] = PORT_PCR_MUX(1U) | PORT_PCR_DSE_MASK;

    PTD->PDDR |= (1<<DC_NUM); // Set as out
    PTD->PDDR |= (1<<RS_NUM);
    PTD->PDDR |= (1<<CS_NUM);
    PTC->PDDR |= (1<<6);
    PTD->PDDR |= (1<<1);
    PTD->PSOR |= (1<<RS_NUM);
    PTD->PSOR |= (1<<DC_NUM);
    PTD->PSOR |= (1<<CS_NUM);

    systick_init();
    dma_init();
    adc_init();
    #ifdef ENABLE_USB_DEBUG
    usb_init();
    #endif
    lcd_init();
    dac_init();
    // Let the DMA do any setup work needed
    tsk_init();
    while(1){
        tsk_main();
    }
    return 1;
}

/* EOF */