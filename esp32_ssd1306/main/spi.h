#ifndef SPI
#define SPI

#include "driver/gpio.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>

void spi_master_init();

bool spi_master_write_byte(const uint8_t *Data, size_t DataLength);
bool spi_master_write_commands(const uint8_t *Commands, size_t DataLength);

bool spi_master_write_data(const uint8_t *Data, size_t DataLength);
bool spi_master_write_command(uint8_t Command);
#endif
