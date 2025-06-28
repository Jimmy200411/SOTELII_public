#ifndef display_h
#define display_h

#include <inttypes.h>

#include "driver/spi_master.h"

#include "gpio.h"

struct spi
{
    spi_device_handle_t spi_handle;
    spi_transaction_t trx;

    uint8_t tx_buffer[16];
    uint8_t rx_buffer[16];
};

int spi_init(struct spi * this);

int spi_transfer(struct spi * this, uint8_t byte);

#endif
