#ifndef CONTEXT_H
#define CONTEXT_H

#include <json-c/json.h>

typedef struct {
    int client_fd;

    struct json_object *obj;
    struct AppState *app;
} Context;

#endif