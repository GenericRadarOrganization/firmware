#include "MKL26Z4.h"
#include "lcd.h"
#include "dma.h"
#include "systick.h"
#include "debug.h"
#include "config.h"

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

static uint8_t rotation = 0;
static uint8_t tabcolor = INITR_BLACKTAB;
static uint8_t _width, _height = 0;
static uint8_t xstart, ystart, colstart, rowstart = 0;

static uint8_t Rcmd1[] = {    // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
        0x00, 0x00,             //     XSTART = 0
        0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
        0x00, 0x00,             //     XSTART = 0
        0x00, 0x9F },           //     XEND = 159
Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

static uint8_t writeByte(uint8_t c)
{
  uint8_t dummy_read;
    // Wait until SPI is ready
    if(SPI0->S & SPI_S_SPRF_MASK){
      dummy_read = SPI0->DL;
    }
    while(!(SPI0->S & SPI_S_SPTEF_MASK));
    SPI0->DL = c;
    return dummy_read;
}

static void writeCommand(uint8_t c)
{
    PTC->PCOR |= (1<<DC_NUM);
    PTC->PCOR |= (1<<CS_NUM);
    // Location in memory will be valid until function ends.
    // We have to wait
    //dma_spi_tx_wait(&c,1);
    writeByte(c);
    PTC->PSOR |= (1<<CS_NUM);
}

static void writeData(uint8_t c)
{
    PTC->PSOR |= (1<<DC_NUM);
    PTC->PCOR |= (1<<CS_NUM);
    // Location in memory will be valid until function ends.
    // We have to wait
    //dma_spi_tx_wait(c,len);
    writeByte(c);
    PTC->PSOR |= (1<<CS_NUM);
}

static void commandList(uint8_t* addr)
{
    uint8_t numCommands, numArgs;
    uint32_t ms, waittime;

    numCommands = *addr++;
    while(numCommands--){
        writeCommand(*addr++);
        numArgs = *addr++;
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        while(numArgs--){
            writeData(*addr++);
        }
        if(ms){
            ms=*addr++;
            if(ms==255){
                ms=500;
            }
            waittime=millis()+ms;
            while(millis()<waittime);
        }
    }
    
}

// This is the only thing that will use SPI, so we can be lazy
// and just init spi in lcd_init
void lcd_init(void)
{
    uint32_t waittime;
    // Pull RS & CS low
    PTC->PCOR |= (1<<CS_NUM);

    PTD->PSOR |= (1<<RS_NUM);
    waittime=millis()+500;
    while(millis()<waittime);

    PTD->PCOR |= (1<<RS_NUM);
    waittime=millis()+500;
    while(millis()<waittime);
    
    PTD->PSOR |= (1<<RS_NUM);
    waittime=millis()+500;
    while(millis()<waittime);

    commandList(Rcmd1);
    commandList(Rcmd2red);
    commandList(Rcmd3);

    lcd_setRotation(0);
}

void lcd_setRotation(uint8_t m) {
  
    writeCommand(ST7735_MADCTL);
    rotation = m % 4; // can't be higher than 3
    switch (rotation) {
     case 0:
       if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
         writeData(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
       } else {
         writeData(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
       }
  
       if (tabcolor == INITR_144GREENTAB) {
         _height = ST7735_TFTHEIGHT_128;
         _width  = ST7735_TFTWIDTH_128;
       } else if (tabcolor == INITR_MINI160x80)  {
         _height = ST7735_TFTHEIGHT_160;
         _width = ST7735_TFTWIDTH_80;
       } else {
         _height = ST7735_TFTHEIGHT_160;
         _width  = ST7735_TFTWIDTH_128;
       }
       xstart = colstart;
       ystart = rowstart;
       break;
     case 1:
       if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
         writeData(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
       } else {
         writeData(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
       }
  
       if (tabcolor == INITR_144GREENTAB)  {
         _width = ST7735_TFTHEIGHT_128;
         _height = ST7735_TFTWIDTH_128;
       } else if (tabcolor == INITR_MINI160x80)  {
         _width = ST7735_TFTHEIGHT_160;
         _height = ST7735_TFTWIDTH_80;
       } else {
         _width = ST7735_TFTHEIGHT_160;
         _height = ST7735_TFTWIDTH_128;
       }
       ystart = colstart;
       xstart = rowstart;
       break;
    case 2:
       if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
         writeData(MADCTL_RGB);
       } else {
         writeData(MADCTL_BGR);
       }
  
       if (tabcolor == INITR_144GREENTAB) {
         _height = ST7735_TFTHEIGHT_128;
         _width  = ST7735_TFTWIDTH_128;
       } else if (tabcolor == INITR_MINI160x80)  {
         _height = ST7735_TFTHEIGHT_160;
         _width = ST7735_TFTWIDTH_80;
       } else {
         _height = ST7735_TFTHEIGHT_160;
         _width  = ST7735_TFTWIDTH_128;
       }
       xstart = colstart;
       ystart = rowstart;
       break;
     case 3:
       if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
         writeData(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
       } else {
         writeData(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
       }
  
       if (tabcolor == INITR_144GREENTAB)  {
         _width = ST7735_TFTHEIGHT_128;
         _height = ST7735_TFTWIDTH_128;
       } else if (tabcolor == INITR_MINI160x80)  {
         _width = ST7735_TFTHEIGHT_160;
         _height = ST7735_TFTWIDTH_80;
       } else {
         _width = ST7735_TFTHEIGHT_160;
         _height = ST7735_TFTWIDTH_128;
       }
       ystart = colstart;
       xstart = rowstart;
       break;
    }
  }