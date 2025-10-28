#include "esp_log.h"
#include "http.h"
#include <cstdlib>
#include <cstring>
#include <string>
#define TAG "HTTP_PARSING"
#include "http_parsing.h"

void apply_header_field(Header *header, string field_name, string value) {

  ESP_LOGI(TAG, "apply_header_field: '%s', value: '%s'", field_name.c_str(),
           value.c_str());

  FieldType field;
  if (auto content_type = FIELD_TYPE_MAP.find(field_name);
      content_type != FIELD_TYPE_MAP.end()) {
    field = content_type->second;
  } else {
    ESP_LOGE(TAG, "field with name: '%s' isn't supported! value: '%s'",
             field_name.c_str(), value.c_str());
    return;
  }
  switch (field) {
  case Host:
    header->host = value;
    break;
  case UserAgent:
    header->user_agent = value;
    break;
  case Content_Type:
    if (auto content_type = CONTENT_TYPE_MAP.find(value);
        content_type != CONTENT_TYPE_MAP.end()) {
      header->content_type = content_type->second;
    } else {
      ESP_LOGE(TAG, "content type with name '%s' is not supported!", value);
    }
    break;
  case Accept:
    header->accept = value;
    break;
  }
}
void handle_header_line(Header *output_header, char *line_buffer,
                        int line_len) {

  string line_str(line_buffer);
  size_t space_pos = line_str.find(':');
  string field_name = line_str.substr(0, space_pos);
  field_name.erase(0, 1);

  string value = line_str.substr(space_pos, 9999999999);
  value.erase(0, 2);

  apply_header_field(output_header, field_name, value);
}
void handle_first_header_line(Header *output_header, char *line_buffer,
                              int line_len) {
  string line_string(line_buffer, line_len);
  if (strncmp(line_buffer, "HTTP/", 5) == 0) {
    // INFO: Response
    output_header->is_request = false;
    // HTTP/1.x

    char num_char = line_buffer[7];
    output_header->version = num_char - '0';

  } else {
    // INFO: Request
    output_header->is_request = true;

    size_t space_pos = line_string.find(' ');
    string method_type_name = line_string.substr(0, space_pos); // "GET"

    if (auto content_type = METHOD_TYPE_MAP.find(method_type_name);
        content_type != METHOD_TYPE_MAP.end()) {
      ESP_LOGI(TAG, "method name: '%s' , type num: '%d'",
               method_type_name.c_str(), content_type->second);
      output_header->method_type = content_type->second;
    } else {
      ESP_LOGI(TAG, "method type with name '%s' is not supported!",
               method_type_name);
    }

    if (space_pos != string::npos) {
      string rest = line_string.substr(space_pos + 1); // "/index.html HTTP/1.1"
      int method_value_end_index = rest.find("HTTP/");

      // "/index.html HTTP/1.1"
      // ------------^---------
      // -1 because of space separation
      string method_value = rest.substr(0, method_value_end_index - 1);
      output_header->method_value = method_value;

      char num_char = line_buffer[method_value_end_index + 7 + space_pos + 1];
      output_header->version = num_char - '0';

      ESP_LOGI(
          TAG, "METHOD  value: '%s', Version '%d', method_value_end_index '%d'",
          method_value.c_str(), output_header->version, method_value_end_index);
    }
  }
}

Message parse_input_http_data(char *buffer, int bytes_received) {
  const int LINE_BUFFER_LEN = 1000;
  int line_beginning = 0;
  char *line_buffer = (char *)malloc(LINE_BUFFER_LEN);

  Header header = {};
  for (int i = 0; i < bytes_received; i++) {
    if (buffer[i] == '\r' && buffer[1 + i] == '\n') {
      i++;
      int line_len = i - line_beginning;
      if (line_len > LINE_BUFFER_LEN) {
        ESP_LOGE(TAG, "the line length was bigger than the buffer length");
      }

      // INFO: Copy the line from buffer[line_beginning..i) into line buffer
      memcpy(line_buffer, buffer + line_beginning, line_len);
      line_buffer[line_len] = '\0'; // Null-terminate it for use as string

      ESP_LOGI(TAG, "Line: %s", line_buffer);
      // WARN: the header will contain arbitrary data at first because of lac of
      // initialization. so just write to it, no reads!
      if (line_beginning == 0) {
        handle_first_header_line(&header, line_buffer, line_len);
      } else {
        handle_header_line(&header, line_buffer, line_len);
      }

      line_beginning = i;
      i--;
      if (buffer[i + 2] == '\r' && buffer[i + 3] == '\n') {
        i += 2;
        ESP_LOGI(TAG, "FOUND contents:i: %d", i);
        // Contents start headr
        break;
      }
    }
  }

  // INFO: Everything from line_beginning..bytes_received
  // is a content
  int contents_len = bytes_received - line_beginning;
  char *contents = (char *)malloc(contents_len + 1); // +1 for null terminator
  memcpy(contents, buffer + line_beginning, contents_len);
  contents[contents_len] = '\0'; // terminate string

  ESP_LOGI(TAG, "CONTENTS: %s ", contents);
  string contents_str(contents);

  return Message{.header = header, .contents = contents_str};
}
