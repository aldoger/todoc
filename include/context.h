#ifndef CONTEXT_H
#define CONTEXT_H

#include <json-c/json.h>
#include "app_state.h"

typedef struct {
    int client_fd;

    struct json_object *obj;
    AppState *app;
} Context;

#endif