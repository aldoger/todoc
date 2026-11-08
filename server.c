#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#include "include/handler.h"
#include "include/server.h"
#include "include/app_state.h"

#define INITIAL_TOTAL_HANDLERS 10
#define AMOUNT_ADD_HANDLER 10
#define BUFFER_SIZE 1600

Server server_constructor(
    int domain,
    int port,
    int service,
    int protocol,
    int backlog,
    __u_long interface,
    void (*launch_fn)(Server *server),
    AppState *app_state
) {
    Server server = {0};

    // Initialize handler array
    server.handler_capacity = INITIAL_TOTAL_HANDLERS;
    server.handler_count = 0;

    server.handlers = calloc(
        server.handler_capacity,
        sizeof(Handler)
    );

    if (server.handlers == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    server.domain = domain;
    server.port = port;
    server.service = service;
    server.protocol = protocol;
    server.backlog = backlog;
    server.interface = interface;
    server.app_state = app_state;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = socket(domain, service, protocol);

    if (server.socket < 0) {
        perror("socket");
        free(server.handlers);
        exit(EXIT_FAILURE);
    }

    if (bind(
        server.socket,
        (struct sockaddr *)&server.address,
        sizeof(server.address)
    ) < 0) {
        perror("bind");
        close(server.socket);
        free(server.handlers);
        exit(EXIT_FAILURE);
    }

    if (listen(server.socket, server.backlog) < 0) {
        perror("listen");
        close(server.socket);
        free(server.handlers);
        exit(EXIT_FAILURE);
    }

    server.launch = launch_fn;

    return server;
}

void launch(Server *server) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("WAITING FOR REQUEST\n");
        fflush(stdout);

        struct sockaddr_in client_address = {0};
        socklen_t addrlen = sizeof(client_address);

        int client_fd = accept(
            server->socket,
            (struct sockaddr *)&client_address,
            &addrlen
        );

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        ssize_t bytes_read = read(
            client_fd,
            buffer,
            BUFFER_SIZE - 1
        );

        if (bytes_read <= 0) {
            close(client_fd);
            continue;
        }

        buffer[bytes_read] = '\0';

        char *body = strstr(buffer, "\r\n\r\n");

        if (!body) {
            close(client_fd);
            continue;
        }

        body += 4;

        struct json_object *json = NULL;

        if (strstr(buffer, "Content-Type: application/json")) {
            json = json_tokener_parse(body);
        }

        Context ctx = {0};

        ctx.client_fd = client_fd;
        ctx.app = server->app_state;
        ctx.obj = json;

        char method[8];
        char path[1024];

        if (sscanf(buffer, "%7s %1023s", method, path) != 2) {
            if (json) json_object_put(json);
            close(client_fd);
            continue;
        }

        char *q = strchr(path, '?');

        if (q) {
            *q = '\0';
        }

        int handled = 0;

        for (size_t i = 0; i < server->handler_count; i++) {
            if (strcmp(path, server->handlers[i].route) == 0) {
                server->handlers[i].handler(&ctx);
                handled = 1;
                break;
            }
        }

        if (!handled) {
            const char *resp =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "\r\n"
                "404 Not Found\n";

            write(client_fd, resp, strlen(resp));
        }

        if (json) {
            json_object_put(json);
        }

        close(client_fd);
    }
}


void server_add_handler(
    Server *server,
    const char *route,
    HandlerFn handler
) {
    if (server == NULL || route == NULL || handler == NULL) {
        return;
    }

    if (strlen(route) >= sizeof(server->handlers[0].route)) {
        fprintf(stderr, "Route is too long\n");
        return;
    }

    if (server->handler_count >= server->handler_capacity) {

        size_t new_capacity =
            server->handler_capacity + AMOUNT_ADD_HANDLER;

        Handler *new_handlers = realloc(
            server->handlers,
            sizeof(Handler) * new_capacity
        );

        if (new_handlers == NULL) {
            perror("realloc");
            return;
        }

        server->handlers = new_handlers;
        server->handler_capacity = new_capacity;
    }

    Handler *new_handler =
        &server->handlers[server->handler_count];

    strcpy(new_handler->route, route);
    new_handler->handler = handler;

    server->handler_count++;
}

void server_destroy(Server *server)
{
    if (server == NULL) {
        return;
    }

    if (server->socket >= 0) {
        close(server->socket);
        server->socket = -1;
    }

    free(server->handlers);
    server->handlers = NULL;
    server->handler_count = 0;
    server->handler_capacity = 0;
}