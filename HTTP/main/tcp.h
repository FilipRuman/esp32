#ifndef TCP
#define TCP
#include <stdint.h>
void run_tcp(uint32_t port);
void send_data(void *data_ptr, int size);
#endif
