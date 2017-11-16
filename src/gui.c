#include "gui.h"
#include "lcd.h"
#include "adc.h"
#include "debug.h"
#include "buttons.h"
#include "systick.h"
#include "adc_proc.h"
#include "MKL26Z4.h"

#include <stdio.h>

#define ST7735_GRAY lcd_Color565(180,180,180)

typedef struct {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
} line_t;

static line_t cat[] = {
    {0,0,63,31},
    {0,31,63,0},
    {0,0,0,63},
    {63,0,63,63},
    {0,63,31,79},
    {63,63,31,79},
    {0,31,31,79},
    {63,31,31,79}
};

static line_t whiskers[] = {
    {15,47-8,63-24,47-4},
    {15,47,63-22,47},
    {15,47+8,63-20,47+3},
    {111,47-8,63+24,47-4},
    {111,47,63+22,47},
    {111,47+8,63+20,47+3}
};

static uint32_t splashscreen_enter_time;
static gui_state_t state;

static void drawvector(line_t* arr, uint16_t size, uint16_t xo, uint16_t yo, uint16_t color)
{
    int i;
    for(i=0;i<size;i++){
        lcd_drawLine(arr[i].x1+xo,arr[i].y1+yo,arr[i].x2+xo,arr[i].y2+yo,color);
    }
}

static void drawwave(uint16_t x0, uint16_t y0, int16_t* buff, uint16_t buff_len, uint16_t color)
{
    // Assuming we want a width of 64, height of 64
    uint16_t val, i;
    lcd_fillRect(x0,y0-63,96,64,ST7735_BLACK);
    for(i=0;i<96;i++)
    {
        val = *buff / 64;
        lcd_drawPixel(x0+i,y0-val,color);
        buff+=1;
    }
    
}

static void drawtab(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t chamfer, uint16_t color, uint16_t outline)
{
    int i;
    // Draw cornered section
    for(i=0;i<chamfer;i++){
        lcd_drawLine(x0+chamfer-i-1,y0+i,x0+w-1,y0+i,color);
    }
    // Fill middle
    lcd_fillRect(x0,y0+chamfer,w,h-2*chamfer,color);
    // Draw second cornered section
    for(i=0;i<chamfer;i++){
        lcd_drawLine(x0+i,y0+h-chamfer+i,x0+w-1,y0+h-chamfer+i,color);
    }
    // Draw an outline
    lcd_drawLine(x0+chamfer-1,y0,x0+w-1,y0, outline);
    lcd_drawLine(x0,y0+chamfer-1,x0+chamfer-1,y0, outline);
    lcd_drawLine(x0,y0+chamfer-1,x0,y0+h-chamfer, outline);
    lcd_drawLine(x0,y0+h-chamfer,x0+chamfer-1,y0+h-1, outline);
    lcd_drawLine(x0+chamfer-1,y0+h-1,x0+w-1,y0+h-1, outline);
}
void gui_init(void)
{
    lcd_init();
    lcd_setRotation(0);

    // Draw the splashscreen
    lcd_fillScreen(ST7735_RED);
    drawvector(cat,8,32,32,ST7735_WHITE);
    drawvector(whiskers,6,0,32,ST7735_WHITE);
    lcd_drawChars(38,120,"GRO",sizeof("GRO"),ST7735_WHITE,ST7735_WHITE,3);
    lcd_drawChars(5,150,"We can't pick a name",sizeof("We can't pick a name"),ST7735_WHITE,ST7735_WHITE,1);
    // Set up state variables
    splashscreen_enter_time = millis();
    state = GUI_SPLASHSCREEN;
}

void gui_loop(void)
{
    uint8_t buttons = button_state();
    char strbuff[5];
    uint8_t charcnt;

    // Let's run the GUI state machine!
    switch(state){
        case GUI_SPLASHSCREEN:
            if(buttons||(millis()-splashscreen_enter_time)>5000){
                state = GUI_IDLE;
                lcd_fillScreen(ST7735_BLACK);
                drawtab(107,0,20,40,5,ST7735_WHITE, ST7735_GRAY);
                drawtab(107,40,20,40,5,ST7735_WHITE, ST7735_GRAY);
                drawtab(107,80,20,40,5,ST7735_WHITE, ST7735_GRAY);
                drawtab(107,120,20,40,5,ST7735_WHITE, ST7735_GRAY);
            }
            break;
        case GUI_IDLE:
            charcnt = snprintf(strbuff,5,"%04d",adc_proc_get_freq());
            lcd_drawChars(5,5,strbuff,charcnt,ST7735_CYAN,ST7735_BLACK,3);
            lcd_drawChars(3*6*4+5,12,"Hz",2,ST7735_CYAN,ST7735_BLACK,2);
            drawwave(0,128,adc_safe_buffer,ADC_BUFFER_SIZE,ST7735_YELLOW);
            break;
    }
}