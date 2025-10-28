#include "esp_log.h"
#include "http_parsing.h"
#include "http_response_handler.h"
#include "tcp.h"
#include <cstring>
#include <http.h>
#include <sstream>
#include <string>
using std::string;

const std::unordered_map<StatusCode, string> STATUS_CODE_NAMES = {
    {OK, "OK"}, {Bad_Request, "Bad Request"}};

const char *CONTENT_TYPE_VALUES[] = {"text/html", "text/plain"};

const std::unordered_map<string, MethodType> METHOD_TYPE_MAP = {
    {"POST", POST},
    {"GET", Get},
};
const std::unordered_map<string, FieldType> FIELD_TYPE_MAP = {
    {"Host", Host},
    {"User-Agent", UserAgent},
    {"Accept", Accept},
};

extern const std::unordered_map<string, ContentType> CONTENT_TYPE_MAP{
    {
        "text/html",
        HTML,
    },
    {"text/plain", Plain}};

static const char *TAG = "HTTP";

void http_init() {}

void print_x_chars(int offset, char *buffer, int amount_to_print) {
  ESP_LOGI(TAG, "%.*s\n", amount_to_print, buffer + offset);
}
#define RN "\r\n"
string header_to_str(Header *header) {
  std::ostringstream stream;
  stream << "HTTP/1." << header->version << ' ' << header->status_code << ' '
         << STATUS_CODE_NAMES.at(header->status_code) << RN;

  stream << "Content-Type: " << CONTENT_TYPE_VALUES[header->content_type] << RN;
  stream << "Content-Length: " << header->content_length << RN;
  stream << "Accept: " << header->accept << RN;

  stream << RN;

  return stream.str();
}

string message_to_text(Message *message) {
  string header_str = header_to_str(&message->header);
  // return header_str + message->contents;
  return header_str + message->contents;
}

void http_handle_input_data(char *buffer, int bytes_received, int buffer_len) {
  ESP_LOGI(TAG, "http_handle_input_data: '%s'", buffer);

  auto message = parse_input_http_data(buffer, bytes_received);

  ESP_LOGI(
      TAG,
      "parsed message: header: {\n \r method type: '%d' \n \r content type: "
      "'%d' \n \r accept: '%s' \n} ,contests: '%s'",
      message.header.method_type, message.header.content_type,
      message.header.accept.c_str(), message.contents.c_str());

  // Header header = {
  //     .status_code = OK,
  //     .version = 1,
  //     .content_length = 2,
  //     .content_type = Plain,
  //     .accept = "*/*",
  // };
  // Message message = {.header = header, .contents = "WOW"};
  //
  // auto str = message_to_text(&message);
  // const char *out = str.c_str();
  // ESP_LOGI(TAG, "out %s", out);
  //
  // send_data((void *)out, strlen(out));
}
