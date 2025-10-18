#include "freertos/idf_additions.h"
#include "pins.h"
#include "portmacro.h"
#include "spi.h"
#include <stdbool.h>
#include <stdint.h>

#define WIDTH 128
#define HEIGHT 64

#define DRAW_BUFFER_LENGTH WIDTH *HEIGHT / 8
static uint8_t *DRAW_BUFFER;
void redraw_buffer() { spi_master_write_data(DRAW_BUFFER, DRAW_BUFFER_LENGTH); }

void oled_init() {
#define OLED_CMD_DISPLAY_OFF 0xAE
  spi_master_write_command(OLED_CMD_DISPLAY_OFF);
#define OLED_CMD_SET_MUX_RATIO 0xA8
  spi_master_write_command(OLED_CMD_SET_MUX_RATIO);
  if (HEIGHT == 64)
    spi_master_write_command(0x3F);
  else if (HEIGHT == 32)
    spi_master_write_command(0x1F);
#define OLED_CMD_SET_DISPLAY_OFFSET 0xD3
  spi_master_write_command(OLED_CMD_SET_DISPLAY_OFFSET);
  spi_master_write_command(0x00);
#define OLED_CONTROL_BYTE_DATA_STREAM 0x40
  spi_master_write_command(OLED_CONTROL_BYTE_DATA_STREAM);
#define OLED_CMD_SET_SEGMENT_REMAP_1 0xA1
  spi_master_write_command(OLED_CMD_SET_SEGMENT_REMAP_1);
//
// spi_master_write_command( OLED_CMD_SET_SEGMENT_REMAP);
#define OLED_CMD_SET_COM_SCAN_MODE 0xC8
  spi_master_write_command(OLED_CMD_SET_COM_SCAN_MODE);
#define OLED_CMD_SET_DISPLAY_CLK_DIV 0xD5
  spi_master_write_command(OLED_CMD_SET_DISPLAY_CLK_DIV);
  spi_master_write_command(0x80);
#define OLED_CMD_SET_COM_PIN_MAP 0xDA
  spi_master_write_command(OLED_CMD_SET_COM_PIN_MAP);
  if (HEIGHT == 64)
    spi_master_write_command(0x12);
  else
    spi_master_write_command(0x02);
#define OLED_CMD_SET_CONTRAST 0x81
  spi_master_write_command(OLED_CMD_SET_CONTRAST);
  spi_master_write_command(0xFF);
#define OLED_CMD_DISPLAY_RAM 0xA4
  spi_master_write_command(OLED_CMD_DISPLAY_RAM);
#define OLED_CMD_SET_VCOMH_DESELCT 0xDB
  spi_master_write_command(OLED_CMD_SET_VCOMH_DESELCT);
  spi_master_write_command(0x40);
#define OLED_CMD_SET_MEMORY_ADDR_MODE 0x20
  spi_master_write_command(OLED_CMD_SET_MEMORY_ADDR_MODE);
// spi_master_write_command( OLED_CMD_SET_HORI_ADDR_MODE);
#define OLED_CMD_SET_HORIZONTAL_ADDR_MODE 0x00
  spi_master_write_command(OLED_CMD_SET_HORIZONTAL_ADDR_MODE);

#define OLED_CMD_SET_CHARGE_PUMP 0x8D
  spi_master_write_command(OLED_CMD_SET_CHARGE_PUMP);
  spi_master_write_command(0x14);
#define OLED_CMD_DEACTIVATE_SCROLL 0x2E
  spi_master_write_command(OLED_CMD_DEACTIVATE_SCROLL);
//
#define OLED_CMD_DISPLAY_NORMAL 0xA6
  spi_master_write_command(OLED_CMD_DISPLAY_NORMAL);

#define OLED_CMD_DISPLAY_ON 0xAF
  spi_master_write_command(OLED_CMD_DISPLAY_ON);

  DRAW_BUFFER = (uint8_t *)malloc(DRAW_BUFFER_LENGTH);
}
void draw_test_pattern() {
  for (int i = 0; i < DRAW_BUFFER_LENGTH; i++) {
    if (i % 2 == 0)
      DRAW_BUFFER[i] = 0xFF;
    else
      DRAW_BUFFER[i] = 0x00;
  }
  redraw_buffer();
}

/// It sets pixel inside buffer but doesn't draw it on the screen. for that use
/// 'redraw_buffer();'
void set_pixel(int x, int y, bool value) {
  int index = x / 8 + y * WIDTH / 8;
  uint8_t mask = 1 << (x % 8);
  if (value)
    DRAW_BUFFER[index] |= mask;
  else
    DRAW_BUFFER[index] &= ~mask;
}

void ssd_init() {
  spi_master_init();
  oled_init();
  draw_test_pattern();
}
