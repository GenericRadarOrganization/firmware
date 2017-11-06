/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows 
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "kinetis.h"

// Flash Security Setting. On Teensy 3.2, you can lock the MK20 chip to prevent
// anyone from reading your code.  You CAN still reprogram your Teensy while
// security is set, but the bootloader will be unable to respond to auto-reboot
// requests from Arduino. Pressing the program button will cause a full chip
// erase to gain access, because the bootloader chip is locked out.  Normally,
// erase occurs when uploading begins, so if you press the Program button
// accidentally, simply power cycling will run your program again.  When
// security is locked, any Program button press causes immediate full erase.
// Special care must be used with the Program button, because it must be made
// accessible to initiate reprogramming, but it must not be accidentally
// pressed when Teensy Loader is not being used to reprogram.  To set lock the
// security change this to 0xDC.  Teensy 3.0 and 3.1 do not support security lock.
#define FSEC 0xDE

// Flash Options
#define FOPT 0xF9

extern unsigned long _estack;

extern unsigned long _stext;
extern unsigned long _etext;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;

// should reset handler exit, the hard_fault_isr will be called
void ResetHandler(void);
extern int main (void);

__attribute__ ((section(".startup")))
void init_data_bss()
{
    uint32_t *src = &_etext;
    uint32_t *dest = &_sdata;
    while (dest < &_edata) *dest++ = *src++;
    dest = &_sbss;
    while (dest < &_ebss) *dest++ = 0;
}

__attribute__ ((section(".startup")))
void fault_isr(void)
{
	while (1) ;
}

__attribute__ ((section(".startup")))
void unused_isr(void)
{
	fault_isr();
}

void nmi_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void hard_fault_isr(void)	__attribute__ ((weak, alias("fault_isr")));
void memmanage_fault_isr(void)	__attribute__ ((weak, alias("fault_isr")));
void bus_fault_isr(void)	__attribute__ ((weak, alias("fault_isr")));
void usage_fault_isr(void)	__attribute__ ((weak, alias("fault_isr")));
void svcall_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void debugmonitor_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void pendablesrvreq_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void systick_isr(void)		__attribute__ ((weak, alias("unused_isr")));

void dma_ch0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch2_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch3_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch4_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch5_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch6_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch7_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch8_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch9_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch10_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch11_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch12_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch13_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch14_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_ch15_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dma_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void mcm_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void randnum_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void flash_cmd_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void flash_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void low_voltage_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void wakeup_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void watchdog_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void i2c0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void i2c1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void i2c2_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void i2c3_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void spi0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void spi1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void spi2_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void sdhc_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void enet_timer_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void enet_tx_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void enet_rx_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void enet_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can0_message_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can0_bus_off_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can0_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can0_tx_warn_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can0_rx_warn_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can0_wakeup_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can1_message_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can1_bus_off_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can1_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can1_tx_warn_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can1_rx_warn_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void can1_wakeup_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void i2s0_tx_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void i2s0_rx_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void i2s0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void uart0_lon_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart0_status_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart0_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart1_status_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart1_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart2_status_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart2_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart3_status_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart3_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart4_status_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart4_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart5_status_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void uart5_error_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void lpuart0_status_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void adc0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void adc1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void cmp0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void cmp1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void cmp2_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void cmp3_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void ftm0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void ftm1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void ftm2_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void ftm3_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void tpm0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void tpm1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void tpm2_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void cmt_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void rtc_alarm_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void rtc_seconds_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void pit_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void pit0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void pit1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void pit2_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void pit3_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void pdb_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void usb_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void usb_charge_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void usbhs_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void usbhs_phy_isr(void)	__attribute__ ((weak, alias("unused_isr")));
void dac0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void dac1_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void tsi0_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void mcg_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void lptmr_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void porta_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void portb_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void portc_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void portd_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void porte_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void portcd_isr(void)		__attribute__ ((weak, alias("unused_isr")));
void software_isr(void)		__attribute__ ((weak, alias("unused_isr")));

__attribute__ ((section(".vectors"), used))
void (* const _VectorsFlash[NVIC_NUM_INTERRUPTS+16])(void) =
{
	(void (*)(void))((unsigned long)&_estack),	//  0 ARM: Initial Stack Pointer
	ResetHandler,					//  1 ARM: Initial Program Counter
	nmi_isr,					//  2 ARM: Non-maskable Interrupt (NMI)
	hard_fault_isr,					//  3 ARM: Hard Fault
	memmanage_fault_isr,				//  4 ARM: MemManage Fault
	bus_fault_isr,					//  5 ARM: Bus Fault
	usage_fault_isr,				//  6 ARM: Usage Fault
	fault_isr,					//  7 --
	fault_isr,					//  8 --
	fault_isr,					//  9 --
	fault_isr,					// 10 --
	svcall_isr,					// 11 ARM: Supervisor call (SVCall)
	debugmonitor_isr,				// 12 ARM: Debug Monitor
	fault_isr,					// 13 --
	pendablesrvreq_isr,				// 14 ARM: Pendable req serv(PendableSrvReq)
	systick_isr,					// 15 ARM: System tick timer (SysTick)

	dma_ch0_isr,					// 16 DMA channel 0 transfer complete
	dma_ch1_isr,					// 17 DMA channel 1 transfer complete
	dma_ch2_isr,					// 18 DMA channel 2 transfer complete
	dma_ch3_isr,					// 19 DMA channel 3 transfer complete
	unused_isr,					// 20 --
	flash_cmd_isr,					// 21 Flash Memory Command complete
	low_voltage_isr,				// 22 Low-voltage detect/warning
	wakeup_isr,					// 23 Low Leakage Wakeup
	i2c0_isr,					// 24 I2C0
	i2c1_isr,					// 25 I2C1
	spi0_isr,					// 26 SPI0
	spi1_isr,					// 27 SPI1
	uart0_status_isr,				// 28 UART0 status & error
	uart1_status_isr,				// 29 UART1 status & error
	uart2_status_isr,				// 30 UART2 status & error
	adc0_isr,					// 31 ADC0
	cmp0_isr,					// 32 CMP0
	ftm0_isr,					// 33 FTM0
	ftm1_isr,					// 34 FTM1
	ftm2_isr,					// 35 FTM2
	rtc_alarm_isr,					// 36 RTC Alarm interrupt
	rtc_seconds_isr,				// 37 RTC Seconds interrupt
	pit_isr,					// 38 PIT Both Channels
	i2s0_isr,					// 39 I2S0 Transmit & Receive
	usb_isr,					// 40 USB OTG
	dac0_isr,					// 41 DAC0
	tsi0_isr,					// 42 TSI0
	mcg_isr,					// 43 MCG
	lptmr_isr,					// 44 Low Power Timer
	software_isr,					// 45 Software interrupt
	porta_isr,					// 46 Pin detect (Port A)
	portcd_isr,					// 47 Pin detect (Port C and D)
};

__attribute__ ((section(".flashconfig"), used))
const uint8_t flashconfigbytes[16] = 
{
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, FSEC, FOPT, 0xFF, 0xFF
};


__attribute__ ((section(".startup")))
void ResetHandler(void)
{
	SIM_COPC = 0;  // disable the watchdog

	// release I/O pins hold, if we woke up from VLLS mode
	if (PMC_REGSC & PMC_REGSC_ACKISO) PMC_REGSC |= PMC_REGSC_ACKISO;

        // since this is a write once register, make it visible to all F_CPU's
        // so we can into other sleep modes in the future at any speed
	SMC_PMPROT = SMC_PMPROT_AVLP | SMC_PMPROT_ALLS | SMC_PMPROT_AVLLS;
    
        init_data_bss();

	// hardware always starts in FEI mode
	//  C1[CLKS] bits are written to 00
	//  C1[IREFS] bit is written to 1
	//  C6[PLLS] bit is written to 0
        // MCG_SC[FCDIV] defaults to divide by two for internal ref clock
        // I tried changing MSG_SC to divide by 1, it didn't work for me
        // enable capacitors for crystal
        OSC0_CR = OSC_SC8P | OSC_SC2P | OSC_ERCLKEN;

	// enable osc, 8-32 MHz range, low power mode
	MCG_C2 = MCG_C2_RANGE0(2) | MCG_C2_EREFS;
	// switch to crystal as clock source, FLL input = 16 MHz / 512
	MCG_C1 =  MCG_C1_CLKS(2) | MCG_C1_FRDIV(4);
	// wait for crystal oscillator to begin
	while ((MCG_S & MCG_S_OSCINIT0) == 0) ;
	// wait for FLL to use oscillator
	while ((MCG_S & MCG_S_IREFST) != 0) ;
	// wait for MCGOUT to use oscillator
	while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2)) ;

	// now in FBE mode
	//  C1[CLKS] bits are written to 10
	//  C1[IREFS] bit is written to 0
	//  C1[FRDIV] must be written to divide xtal to 31.25-39 kHz
	//  C6[PLLS] bit is written to 0
	//  C2[LP] is written to 0
	// if we need faster than the crystal, turn on the PLL

	// config PLL input for 16 MHz Crystal / 4 = 4 MHz
	MCG_C5 = MCG_C5_PRDIV0(3);

        // config PLL for 96 MHz output
	MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0(0); 

	// wait for PLL to start using xtal as its input
	while (!(MCG_S & MCG_S_PLLST)) ;
	// wait for PLL to lock
	while (!(MCG_S & MCG_S_LOCK0)) ;
	// now we're in PBE mode

	// now program the clock dividers
	// config divisors: 48 MHz core, 48 MHz bus, 24 MHz flash, USB = 96 / 2
	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(1) | SIM_CLKDIV1_OUTDIV4(1);

	// switch to PLL as clock source, FLL input = 16 MHz / 512
	MCG_C1 = MCG_C1_CLKS(0) | MCG_C1_FRDIV(4);
	// wait for PLL clock to be used
	while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3)) ;
	// now we're in PEE mode
	// USB uses PLL clock, trace is CPU clock, CLKOUT=OSCERCLK0
	SIM_SOPT2 = SIM_SOPT2_USBSRC | SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_CLKOUTSEL(6)
		| SIM_SOPT2_UART0SRC(1) | SIM_SOPT2_TPMSRC(1);

	// initialize the SysTick counter
	SYST_RVR = (F_CPU / 1000) - 1;
	SYST_CVR = 0;
	SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
	SCB_SHPR3 = 0x20200000;  // Systick = priority 32

	__enable_irq();

	main();
}

