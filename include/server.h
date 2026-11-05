#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include "handler.h"
#include <json-c/json.h>

#define BUFFER_SIZE 16000
#define MAX_HANDLERS 100

struct Server {
    int domain;
    int port;
    int service;
    int protocol;
    int backlog;
    __u_long interface;

    int socket;
    struct sockaddr_in address;

    struct Handler handlers[MAX_HANDLERS];
    int handler_count;

    void (*launch)(struct Server *server);

    void *app_state;
};

struct Server server_constructor(int domain, int port, int service, int protocol, int backlog, __u_long interface, void (*launch)(struct Server *server), void *app_state);
void server_add_handler(struct Server *server, const char *route, void (*handler)(Context *));
void launch(struct Server *server);

#endif
