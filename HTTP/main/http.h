
#ifndef HTTP
#define HTTP
void http_init();

void http_handle_input_data(char *buffer, int bytes_received, int buffer_len);
#endif
