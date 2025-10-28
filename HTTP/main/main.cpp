#include "esp_err.h"
#include "http.h"
#include "http_parser.h"
#include "tcp.h"
#include "wifi.h"

extern "C" void app_main() {
  start_wifi();
  run_tcp(80);
  http_init();
}
