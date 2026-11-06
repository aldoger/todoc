#ifndef HANDLER_H
#define HANDLER_H

#include "context.h"

typedef void (*HandlerFn)(Context *ctx);

typedef struct {
    char route[64];
    HandlerFn handler;
} Handler;

#endif