#include "esp_log.h"
static const char *TAG = "HTTP";

void http_init() {}

void print_x_chars(int offset, char *buffer, int amount_to_print) {
  ESP_LOGI(TAG, "%.*s\n", amount_to_print, buffer + offset);
}

void http_handle_input_data(char *buffer, int bytes_received, int buffer_len) {
  ESP_LOGI(TAG, "http_handle_input_data");
  int len = (bytes_received / 8);
  for (int i = 0; i < len; i++) {
    print_x_chars(i * 8, buffer, 8);
  }
  print_x_chars((len) * 8, buffer, bytes_received % 8);
}
