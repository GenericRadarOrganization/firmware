#include "dma.h"
#include "MKL26Z4.h"
#include "config.h"
#include "debug.h"

static int16_t* read_dest;
static uint32_t read_size;

static uint16_t* dma_buff;

void dma_init(void)
{
    // Gate the clock
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}

static void configure_dac_out(uint16_t* src)
{
    DMA0->DMA[DMA_DAC_OUT_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[DMA_DAC_OUT_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[DMA_DAC_OUT_CHANNEL].SAR = (uint32_t)(src); // Transfer from out adc sc1 value
    // Things to be hopeful for: Successful 16bit writes to DAC0->DAT[0]
    DMA0->DMA[DMA_DAC_OUT_CHANNEL].DAR = (uint32_t)(&(DAC0->DAT[0].DATL)); // to ADC0 sc1
    DMA0->DMA[DMA_DAC_OUT_CHANNEL].DSR_BCR = DMA_DSR_BCR_BCR(DAC_BUFFER_SIZE*2); // Set the byte count (size of dest ptr*2)
    DMA0->DMA[DMA_DAC_OUT_CHANNEL].DCR = DMA_DCR_CS_MASK | // Enable Cycle Steal
                                         DMA_DCR_D_REQ_MASK | // Disable hardware requests on completion
                                         DMA_DCR_SINC_MASK | // Source increments
                                         DMA_DCR_ERQ_MASK |
                                         DMA_DCR_EINT_MASK | // Enable ints so we can restart it
                                         DMA_DCR_SSIZE(2) | // 16bit
                                         DMA_DCR_DSIZE(2);
}

static void configure_adc_read()
{
    DMA0->DMA[DMA_ADC_RESULT_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[DMA_ADC_RESULT_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[DMA_ADC_RESULT_CHANNEL].SAR = (uint32_t)&ADC0_RA;
    DMA0->DMA[DMA_ADC_RESULT_CHANNEL].DAR = (uint32_t)read_dest;
    DMA0->DMA[DMA_ADC_RESULT_CHANNEL].DSR_BCR = DMA_DSR_BCR_BCR(read_size*2); // Set the byte count (size of dest ptr*2)
    DMA0->DMA[DMA_ADC_RESULT_CHANNEL].DCR = DMA_DCR_CS_MASK | // Enable Cycle Steal
                                            DMA_DCR_DINC_MASK | // Set destination increment
                                            DMA_DCR_D_REQ_MASK | // Disable hardware requests on completion
                                            DMA_DCR_ERQ_MASK |
                                            DMA_DCR_SSIZE(2) | // 16bit
                                            DMA_DCR_DSIZE(2);
}

// Hardcoded method for enabling the ADC DMA requests
void dma_conf_adc_read(int16_t* dest, uint32_t size)
{
    read_dest = dest;
    read_size = size;

    DMAMUX0_CHCFG(DMA_ADC_RESULT_CHANNEL) = 0; // Set full low for reset
    configure_adc_read(dest, size);
    // Route requests from the ADC to the designated DMA channel
    DMAMUX0_CHCFG(DMA_ADC_RESULT_CHANNEL) = DMAMUX_CHCFG_SOURCE(40);
    DMAMUX0_CHCFG(DMA_ADC_RESULT_CHANNEL) |= DMAMUX_CHCFG_ENBL_MASK;
}

void dma_conf_dac_out(uint16_t* src)
{
    debug_printf("First run DAC DMA\n");
    dma_buff = src;
    DMAMUX0_CHCFG(DMA_DAC_OUT_CHANNEL) = 0; // Set full low for reset
    configure_dac_out(dma_buff);
    // Set up DMA MUX for always-on periodic trigger mode
    // This assumes that the appropriate PIT channel is already configured
    DMAMUX0_CHCFG(DMA_DAC_OUT_CHANNEL) = DMAMUX_CHCFG_SOURCE(61) | DMAMUX_CHCFG_TRIG_MASK;
    DMAMUX0_CHCFG(DMA_DAC_OUT_CHANNEL) |= DMAMUX_CHCFG_ENBL_MASK;

    // Enable interrupts for automatic reset
    NVIC_ClearPendingIRQ(DMA1_IRQn);
    NVIC_EnableIRQ(DMA1_IRQn);
}

uint32_t dma_transfer_complete(void)
{
    uint32_t dma_res_done =  (DMA0->DMA[DMA_ADC_RESULT_CHANNEL].DSR_BCR & DMA_DSR_BCR_DONE_MASK);
    //uint32_t dma_trig_done =  (DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].DSR_BCR & DMA_DSR_BCR_DONE_MASK);
    return (dma_res_done);
}

uint32_t dma_in_error(void)
{
    uint32_t dma_res_error = DMA0->DMA[DMA_ADC_RESULT_CHANNEL].DSR_BCR & (DMA_DSR_BCR_CE_MASK|DMA_DSR_BCR_BES_MASK|DMA_DSR_BCR_BED_MASK);
    return (dma_res_error)!=0;
}

void dma_adc_restart(int16_t* dest)
{
    // Set the destination
    read_dest = dest;
    configure_adc_read();
    //configure_adc_trigger();
}

void dma_ch1_isr(void)
{
    // Start it all over again!
    configure_dac_out(dma_buff);
}