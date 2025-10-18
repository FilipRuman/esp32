#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "ssd-1780.h"
void app_main(void) {
  char *ourTaskName = pcTaskGetName(NULL);
  ESP_LOGI(ourTaskName, "Hello, starting up!\n");
  ssd_init();
}
