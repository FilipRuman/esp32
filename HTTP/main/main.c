#include "esp_err.h"
#include "http.h"
#include "tcp.h"
#include "wifi.h"

void app_main(void) {
  start_wifi();
  run_tcp(80);
  http_init();
}




