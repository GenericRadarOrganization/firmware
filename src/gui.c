#include "gui.h"
#include "lcd.h"
#include "MKL26Z4.h"

typedef struct {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
} line_t;

line_t cat[] = {
    {0,0,63,31},
    {0,31,63,0},
    {0,0,0,63},
    {63,0,63,63},
    {0,63,31,79},
    {63,63,31,79},
    {0,31,31,79},
    {63,31,31,79}
};

line_t whiskers[] = {
    {15,47-8,63-24,47-4},
    {15,47,63-22,47},
    {15,47+8,63-20,47+3},
    {111,47-8,63+24,47-4},
    {111,47,63+22,47},
    {111,47+8,63+20,47+3}
};

static void drawvector(line_t* arr, uint16_t size, uint16_t xo, uint16_t yo, uint16_t color)
{
    int i;
    for(i=0;i<size;i++){
        lcd_drawLine(arr[i].x1+xo,arr[i].y1+yo,arr[i].x2+xo,arr[i].y2+yo,color);
    }
}

void gui_init(void)
{
    lcd_init();
    lcd_setRotation(0);
    lcd_fillScreen(ST7735_RED);
    drawvector(cat,8,32,32,ST7735_WHITE);
    drawvector(whiskers,6,0,32,ST7735_WHITE);
}

void gui_loop(void)
{

}