#include "dma.h"
#include "MKL26Z4.h"
#include "config.h"

static int16_t* read_dest;
static uint32_t read_size;

void dma_init(void)
{
    // Gate the clock
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}

static void configure_dac_out(uint16_t* src)
{
    DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].SAR = (uint32_t)(&src); // Transfer from out adc sc1 value
    DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].DAR = (uint32_t)(&(DAC0->DAT[0].DATL)); // to ADC0 sc1
    DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].DSR_BCR = DMA_DSR_BCR_BCR(512*2); // Set the byte count (size of dest ptr*2)
    DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].DCR = DMA_DCR_CS_MASK | // Enable Cycle Steal
                                             DMA_DCR_D_REQ_MASK | // Disable hardware requests on completion
                                             DMA_DCR_ERQ_MASK |
                                             DMA_DCR_SSIZE(1) |
                                             DMA_DCR_DSIZE(1); // Enable peripheral request
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
                                            DMA_DCR_SSIZE(2) |
                                            DMA_DCR_DSIZE(2); // Enable peripheral request
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
    DMAMUX0_CHCFG(DMA_ADC_TRIGGER_CHANNEL) = 0; // Set full low for reset
    configure_dac_out(src);
    DMAMUX0_CHCFG(DMA_ADC_TRIGGER_CHANNEL) = DMAMUX_CHCFG_SOURCE(60) | DMAMUX_CHCFG_TRIG_MASK;
    DMAMUX0_CHCFG(DMA_ADC_TRIGGER_CHANNEL) |= DMAMUX_CHCFG_ENBL_MASK;
    //NVIC_ClearPendingIRQ(PIT_IRQn);
    //NVIC_EnableIRQ(PIT_IRQn);
    //SIM->SOPT7 |= SIM_SOPT7_ADC0ALTTRGEN_MASK | SIM_SOPT7_ADC0TRGSEL(4);
}

void dma_conf_spi()
{
    DMAMUX0_CHCFG(DMA_SPI_CHANNEL) = 0;
    DMAMUX0_CHCFG(DMA_SPI_CHANNEL) = DMAMUX_CHCFG_SOURCE(17);
    DMAMUX0_CHCFG(DMA_SPI_CHANNEL) |= DMAMUX_CHCFG_ENBL_MASK;
}

void dma_spi_tx(void* src, uint32_t size)
{
    // Hang for a while if the dma is busy - Don't shred any transfers
    while(DMA0->DMA[DMA_SPI_CHANNEL].DSR_BCR & DMA_DSR_BCR_BSY_MASK);

    // Close out any errors/transactions
    DMA0->DMA[DMA_SPI_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[DMA_SPI_CHANNEL].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    // Set start register
    DMA0->DMA[DMA_SPI_CHANNEL].SAR = (uint32_t)src;
    DMA0->DMA[DMA_SPI_CHANNEL].DAR = (uint32_t)&(SPI0->DL);
    DMA0->DMA[DMA_SPI_CHANNEL].DSR_BCR = DMA_DSR_BCR_BCR(size);
    DMA0->DMA[DMA_SPI_CHANNEL].DCR = DMA_DCR_CS_MASK | // Enable Cycle Steal
                                     DMA_DCR_SINC_MASK |
                                     DMA_DCR_D_REQ_MASK | // Disable hardware requests on completion
                                     DMA_DCR_ERQ_MASK |
                                     DMA_DCR_SSIZE(1) |
                                     DMA_DCR_DSIZE(1); // 32 bit copies
}

void dma_spi_tx_wait(void* src, uint32_t size)
{
    dma_spi_tx(src,size);
    while(DMA0->DMA[DMA_SPI_CHANNEL].DSR_BCR & DMA_DSR_BCR_BSY_MASK);    
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
    uint32_t dma_trig_error = DMA0->DMA[DMA_ADC_TRIGGER_CHANNEL].DSR_BCR & (DMA_DSR_BCR_CE_MASK|DMA_DSR_BCR_BES_MASK|DMA_DSR_BCR_BED_MASK);
    return (dma_res_error|dma_trig_error)!=0;
}

void dma_restart(int16_t* dest)
{
    // Set the destination
    read_dest = dest;
    configure_adc_read();
    //configure_adc_trigger();
}