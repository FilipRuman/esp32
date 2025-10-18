#ifndef SSD_1780
#define SSD_1780

#include "driver/gpio.h"
#include "freertos/task.h"
void ssd_init();
void set_pixel(int x, int y, bool value);

#endif
