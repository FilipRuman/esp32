#ifndef HTTP_PARSING
#define HTTP_PARSING

#include "http.h"
Message parse_input_http_data(char *buffer, int bytes_received);

#endif
