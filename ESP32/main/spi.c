#include "spi.h"
#include <string.h>

#include "gpio.h"



int spi_init(struct atmega * this)
{
    spi_bus_config_t spi =
    {
        .miso_io_num     = TFT_MISO,
        .mosi_io_num     = TFT_MOSI,
        .sclk_io_num     = TFT_CLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1
    };

    if (spi_bus_initialize(VSPI_HOST, &spi, SPI_DMA_CH_AUTO) != ESP_OK)
        return 0;

    spi_device_interface_config_t tft =
    {
        .command_bits   = 0,
        .address_bits   = 0,
        .dummy_bits     = 0,
        .clock_speed_hz = 1000000,
        .duty_cycle_pos = 128,      // 50% duty cycle
        .mode           = 0,
        .queue_size     = 3
    };

    if (spi_bus_add_device(VSPI_HOST, &tft, &this->spi_handle) != ESP_OK)
        return 0;

    memset(&this->trx, 0, sizeof(spi_transaction_t));

    this->trx.tx_buffer = this->buffer;
    this->trx.rx_buffer = this->buffer_in;

    return 1;
}


int spi_atmega_transmit(struct atmega * this, uint8_t invoertje)
{
    this->trx.length = 8;   
    this->buffer[0] = invoertje;

    this->trx.tx_buffer = this->buffer;
    this->trx.rx_buffer = this->buffer;


        if (spi_device_polling_transmit
            (this->spi_handle, &this->trx) != ESP_OK)
        {
            printf("SPI write error\n"); return 0;
        }

    return 1;


}

int spi_atmega_receive(struct atmega * this, uint8_t *uitvoertje)
{
    this->trx.length = 8;
    this->buffer[0] = 0x00;

   

    if (spi_device_polling_transmit(this->spi_handle, &this->trx) != ESP_OK)
        {
            printf("SPI receive error\n"); return 0;
        }
    
    *uitvoertje = this->buffer[0];

    return 1;
    
}

