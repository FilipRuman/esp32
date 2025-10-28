#ifndef HTTP
#define HTTP
#include <string>
#include <unordered_map>
using std::string;

typedef enum {
  OK = 200,
  Bad_Request = 400,
  Not_Found = 404,

} StatusCode;

typedef enum { POST, Get } MethodType;
typedef enum { Host, UserAgent, Accept, Content_Type } FieldType;

typedef enum {
  HTML,
  Plain,
} ContentType;

extern const std::unordered_map<string, FieldType> FIELD_TYPE_MAP;
extern const std::unordered_map<string, MethodType> METHOD_TYPE_MAP;
extern const std::unordered_map<StatusCode, string> STATUS_CODE_NAMES;
extern const std::unordered_map<string, ContentType> CONTENT_TYPE_MAP;

typedef struct {
  bool is_request;
  StatusCode status_code;
  int version;
  int content_length;
  ContentType content_type;
  string accept;
  string host;
  string user_agent;
  MethodType method_type;
  string method_value;

} Header;
typedef struct {
  Header header;
  string contents;
} Message;

void http_init();
void http_handle_input_data(char *buffer, int bytes_received, int buffer_len);

#endif
