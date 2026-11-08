#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stddef.h>
#include "handler.h"
#include "app_state.h"

typedef struct Server Server;

struct Server {
    int domain;
    int port;
    int service;
    int protocol;
    int backlog;
    __u_long interface;

    int socket;
    struct sockaddr_in address;

    Handler *handlers;
    size_t handler_count;
    size_t handler_capacity;

    void (*launch)(Server *server);

    AppState *app_state;
};

Server server_constructor(
    int domain,
    int port,
    int service,
    int protocol,
    int backlog,
    __u_long interface,
    void (*launch_fn)(Server *server),
    AppState *app_state
);

void server_add_handler(
    Server *server,
    const char *route,
    HandlerFn handler
);

void launch(Server *server);

void server_destroy(Server *server);


#endif