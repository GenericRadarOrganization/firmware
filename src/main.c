#include "fixup.h"
#include "systick.h"
#include "tsk.h"
#include "adc.h"
#include "dma.h"

#include "usb_dev.h"
#include "usb_serial.h"

#define LED  (1U << 5)

int main(void)
{
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    systick_init();
    dma_init();
    adc_init();
    usb_init();
    tsk_init();
    while(1){
        tsk_main();
    }
    return 1;
}

/* EOF */

