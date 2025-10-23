#include "esp_log.h"
#include "http.h"
#include "lwip/sockets.h"
#include <stdint.h>

// connect to the server and return the result
static const char *TAG = "TCP";
void start_tcp(uint32_t port, int *socket_id, char *addr_str) {
  struct sockaddr_in destAddr;
  destAddr.sin_addr.s_addr =
      htonl(INADDR_ANY);           // Change hostname to network byte order
  destAddr.sin_family = AF_INET;   // Define address family as Ipv4
  destAddr.sin_port = htons(port); // Define PORT
  inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

  int addr_family = AF_INET;     // Define address family as Ipv4
  int ip_protocol = IPPROTO_TCP; // Define protocol as TCP
  *socket_id = socket(addr_family, SOCK_STREAM, ip_protocol);
  if (socket_id < 0) {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return;
  }
  ESP_LOGI(TAG, "Socket created");

  /* Bind a socket to a specific IP + port */
  esp_err_t bind_err =
      bind(*socket_id, (struct sockaddr *)&destAddr, sizeof(destAddr));
  if (bind_err != 0) {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    return;
  }
  ESP_LOGI(TAG, "Socket binded");

  /* Begin listening for clients on socket */
  esp_err_t listen_error = listen(*socket_id, 3);
  if (listen_error != 0) {
    ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
    return;
  }
}
void on_valid_data_input(char *buffer, int bytes_received, int buffer_len) {
  http_handle_input_data(buffer, bytes_received, buffer_len);
}

static int client_socket;
void send_data(void *data_ptr, int size) {
  send(client_socket, data_ptr, size, 0);
}

void tcp_loop(int socket_id, char *addr_str) {

  int buffer_len = 128;
  char rx_buffer[buffer_len]; // char array to store received data
  int bytes_received;         // immediate bytes received

  while (1) {
    struct sockaddr_in sourceAddr; // Large enough for IPv4
    socklen_t addrLen = sizeof(sourceAddr);
    /* Accept connection to incoming client */
    client_socket = accept(socket_id, (struct sockaddr *)&sourceAddr, &addrLen);
    if (client_socket < 0) {
      ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
      break;
    }
    ESP_LOGI(TAG, "Socket accepted");

    // Optionally set O_NONBLOCK
    // If O_NONBLOCK is set then recv() will return, otherwise it will stall
    // until data is received or the connection is lost.
    // fcntl(client_socket,F_SETFL,O_NONBLOCK);

    // Clear rx_buffer, and fill with zero's
    bzero(rx_buffer, sizeof(rx_buffer));
    vTaskDelay(500 / portTICK_PERIOD_MS);
    while (1) {
      ESP_LOGI(TAG, "Waiting for data");
      bytes_received = recv(client_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
      ESP_LOGI(TAG, "Received Data");

      char *data = "Receved\n";
      send_data(data, strlen(data));

      // Error occured during receiving
      if (bytes_received < 0) {
        ESP_LOGI(TAG, "Waiting for data");
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      // Connection closed
      else if (bytes_received == 0) {
        ESP_LOGI(TAG, "Connection closed");
        break;
      }
      // Data received
      else {
        // Get the sender's ip address as string
        if (sourceAddr.sin_family == PF_INET) {
          inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr,
                      addr_str, sizeof(addr_str) - 1);
        }

        rx_buffer[bytes_received] =
            0; // Null-terminate whatever we received and treat like a string
        on_valid_data_input(rx_buffer, bytes_received, buffer_len);

        // Clear rx_buffer, and fill with zero's
        bzero(rx_buffer, sizeof(rx_buffer));
      }
    }
  }
}

// this will not quit until error is encountered!
// so using a separate task might be a good idea
void run_tcp(uint32_t port) {
  int socket_id;
  char addr_str[128]; // char array to store client IP
  start_tcp(port, &socket_id, addr_str);

  ESP_LOGI(TAG, "Socket listening");
  tcp_loop(socket_id, addr_str);
}
