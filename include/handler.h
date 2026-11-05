#ifndef HANDLER_H
#define HANDLER_H

#include "context.h"

typedef void (*HandlerFn)(Context *ctx);

struct Handler {
    char route[64];
    HandlerFn handler;
};

#endif