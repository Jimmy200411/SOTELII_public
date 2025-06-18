#ifndef spi_h
#define spi_h

#include <inttypes.h>

#include "driver/spi_master.h"


struct atmega
{
    spi_device_handle_t spi_handle;
    spi_transaction_t trx;

    uint8_t buffer[1024];
    uint8_t buffer_in[1024];
};

int spi_init(struct atmega * this);
int spi_atmega_transmit(struct atmega * this, uint8_t invoertje);
int spi_atmega_receive(struct atmega * this, uint8_t *uitvoertje);

#endif
