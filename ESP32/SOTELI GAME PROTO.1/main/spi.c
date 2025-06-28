#include "spi.h"
#include "gpio.h"
#include <string.h>


int spi_init(struct spi * this)
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
        .clock_speed_hz = 100000, // 100 kHz
        .duty_cycle_pos = 128,    // 50% duty cycle
        .mode           = 0,
        .queue_size     = 3
    };

    if (spi_bus_add_device(VSPI_HOST, &tft, &this->spi_handle) != ESP_OK)
        return 0;

    memset(&this->trx, 0, sizeof(spi_transaction_t));

    this->trx.tx_buffer = this->tx_buffer;
    this->trx.rx_buffer = this->rx_buffer;

    return 1;
}


int spi_transfer(struct spi * this, uint8_t byte)
{
  
    this->tx_buffer[0]  = byte;
    this->trx.length = 8;

    if (spi_device_polling_transmit(this->spi_handle, &this->trx) != ESP_OK)
    {
        printf("SPI write error (c)\n"); TFT_DC_HI(); return -1;
    }

    return this->rx_buffer[0];
}
/* Deze functie verstuurt 8 bits (length = 8 beneden is in bits) via SPI.
     Tijdens het zenden zal ook worden ontvangen: deze data komt dan in
     dezelfde buffer waarin (in eerste instantie) de verzonden data in stond.

     Gaat het verzenden via SPI mis? Dan retourneert deze functie niet 0 maar
     -1. Gaat het goed? Dan retourneert deze functie de ontvangen byte als
     getal 0 t/m 255. */
