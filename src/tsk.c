// Radar Periodic Task Daemon

#include "MKL26Z4.h"
#include "helpers.h"
#include "systick.h"
#include "usb_serial.h"

#define LED  (1U << 5)

typedef struct {
    void (*init)(void);
    void (*main)(void);
    uint32_t next_run;
    uint32_t period;
} task_t;


static void idle_init()
{
    // Enable PORTC clock
    SIM->SCGC5 = SIM_SCGC5_PORTC_MASK;
    // Configure PORTC pinmux
    PORTC->PCR[5] = PORT_PCR_MUX(1U);
    // Set LED direction
    PTC->PDDR |= LED;
    //PTC->PSOR = LED;
}

static void idle_poll()
{
    // Toggle LED
    PTC->PTOR = LED;
    usb_serial_write("ASDF\n",sizeof("ASDF\n"));
}

static task_t task_table[] = {
    {idle_init,idle_poll,0,1000}
};

void tsk_init(void){
    uint8_t i;
    for(i=0;i<cnt_of_array(task_table);i++)
    {
        task_table[i].init();
        task_table[i].next_run = 0;
    }
}

void tsk_main(void){
    uint8_t i;
    for(i=0;i<cnt_of_array(task_table);i++)
    {
        if(task_table[i].next_run<=millis()){
            // Call tasks main function
            task_table[i].main();
            
            // Mark next run time
            task_table[i].next_run=millis()+task_table[i].period;
        }
    }
}