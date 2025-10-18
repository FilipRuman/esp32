#include <stdint.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "hal/spi_types.h"
#include <stdio.h>

#define MOSI 21
#define CLK 22
#define CS 5
#define DC 4
#define RST 2

#define GPIO_LED 2

#include "freertos/task.h"

#include "esp_log.h"

#if CONFIG_SPI2_HOST
#define HOST_ID SPI2_HOST
#elif CONFIG_SPI3_HOST
#define HOST_ID SPI3_HOST
#else
#define HOST_ID SPI2_HOST // If i2c is selected
#endif

#define TAG "SSD1306"
static spi_device_handle_t device_handle;
void spi_master_init() {
  esp_err_t ret;

  // ----- SETUP PINS -----
  gpio_reset_pin(CS);
  gpio_set_direction(CS, GPIO_MODE_OUTPUT);
  gpio_set_level(CS, 0);

  gpio_reset_pin(DC);
  gpio_set_direction(DC, GPIO_MODE_OUTPUT);
  gpio_set_level(DC, 0);

  if (RST >= 0) {
    gpio_reset_pin(RST);
    gpio_set_direction(RST, GPIO_MODE_OUTPUT);
    gpio_set_level(RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(RST, 1);
  }

  // ----- BUS INITIALIZATION -----
  spi_bus_config_t spi_bus_config = {.mosi_io_num = MOSI,
                                     .miso_io_num = -1,
                                     .sclk_io_num = CLK,
                                     .quadwp_io_num = -1,
                                     .quadhd_io_num = -1,
                                     .max_transfer_sz = 0,
                                     .flags = 0};

  ESP_LOGI("spi", "SPI HOST_ID=%d", HOST_ID);
  ret = spi_bus_initialize(HOST_ID, &spi_bus_config, SPI_DMA_CH_AUTO);
  ESP_LOGI("spi", "spi_bus_initialize=%d", ret);

  // ----- ADD SPI DEVICE -----
  spi_device_interface_config_t devcfg;
  memset(&devcfg, 0, sizeof(spi_device_interface_config_t));
  devcfg.clock_speed_hz = 1 * 1000000; // 1 MHz
  devcfg.spics_io_num = CS;
  devcfg.queue_size = 1;

  ret = spi_bus_add_device(HOST_ID, &devcfg, &device_handle);
  ESP_LOGI("spi", "spi_bus_add_device=%d", ret);
}

bool spi_master_write_byte(const uint8_t *Data, size_t DataLength) {
  spi_transaction_t SPITransaction;

  if (DataLength > 0) {
    memset(&SPITransaction, 0, sizeof(spi_transaction_t));
    SPITransaction.length = DataLength * 8;
    SPITransaction.tx_buffer = Data;
    spi_device_transmit(device_handle, &SPITransaction);
  }

  return true;
}
#define SPI_COMMAND_MODE 0
bool spi_master_write_commands(const uint8_t *Commands, size_t DataLength) {
  gpio_set_level(DC, SPI_COMMAND_MODE);
  return spi_master_write_byte(Commands, DataLength);
}

#define SPI_DATA_MODE 1
bool spi_master_write_data(const uint8_t *Data, size_t DataLength) {
  gpio_set_level(DC, SPI_DATA_MODE);
  return spi_master_write_byte(Data, DataLength);
}

bool spi_master_write_command(uint8_t Command) {
  static uint8_t CommandByte = 0;
  CommandByte = Command;
  return spi_master_write_commands(&CommandByte, 1);
}
