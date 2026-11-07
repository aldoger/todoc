#ifndef RESPONSE_H
#define RESPONSE_H

#include <json-c/json.h>

typedef enum {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_BAD_REQUEST = 400,
    HTTP_NOT_FOUND = 404,
    HTTP_INTERNAL_SERVER_ERROR = 500
} StatusCode;

typedef struct {
    struct json_object *obj;
    StatusCode status;
    char *message;
} Response;

Response *create_response(
    struct json_object *obj,
    StatusCode status,
    const char *message
);

struct json_object *response_to_json_object(const Response *response);

void destroy_response(Response *response);

#endif