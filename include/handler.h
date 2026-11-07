#ifndef HANDLER_H
#define HANDLER_H

#include "context.h"
#include "response.h"

typedef void (*HandlerFn)(Context *ctx);

typedef struct {
    char route[64];
    HandlerFn handler;
} Handler;

int send_response_data(int client_fd, const Response *res);

#endif