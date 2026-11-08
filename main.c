#include "include/server.h"
#include "include/response.h"
#include "include/context.h"
#include "include/app_state.h"

#include <json-c/json.h>
#include <stdio.h>

void get_todos(Context *ctx)
{
    AppState *app = ctx->app;

    if (app == NULL) {
        Response *res = create_response(
            NULL,
            HTTP_INTERNAL_SERVER_ERROR,
            "Application state is unavailable"
        );

        if (res != NULL) {
            send_response_data(ctx->client_fd, res);
            destroy_response(res);
        }

        return;
    }

    struct json_object *todos_json = json_object_new_array();

    if (todos_json == NULL) {
        return;
    }

    for (size_t i = 0; i < app->todo_count; i++) {
        Todo *todo = &app->todos[i];

        struct json_object *todo_json = json_object_new_object();

        if (todo_json == NULL) {
            json_object_put(todos_json);
            return;
        }

        json_object_object_add(
            todo_json,
            "priority",
            json_object_new_int(todo->priority)
        );

        json_object_object_add(
            todo_json,
            "desc",
            json_object_new_string(todo->desc ? todo->desc : "")
        );

        json_object_array_add(todos_json, todo_json);
    }

    Response *res = create_response(
        todos_json,
        HTTP_OK,
        "Todos retrieved successfully"
    );

    // Release our reference. Response maintains its own reference.
    json_object_put(todos_json);

    if (res == NULL) {
        return;
    }

    send_response_data(ctx->client_fd, res);

    destroy_response(res);
}

void add_todo(Context *ctx) 
{
    AppState *app = ctx->app;

    if (app == NULL) {
        Response *res = create_response(
            NULL,
            HTTP_INTERNAL_SERVER_ERROR,
            "Application state is unavailable"
        );

        if (res != NULL) {
            send_response_data(ctx->client_fd, res);
            destroy_response(res);
        }

        return;
    }

    struct json_object *req = ctx->obj;
    Todo new_todo = {0};
    
    struct json_object *priority_obj = NULL;
    struct json_object *desc_obj = NULL;

    if (req == NULL || !json_object_is_type(req, json_type_object)) {
        return;
    }

    if (!json_object_object_get_ex(req, "priority", &priority_obj) ||
        priority_obj == NULL ||
        !json_object_is_type(priority_obj, json_type_int)) {
        return;
    }

    if (!json_object_object_get_ex(req, "desc", &desc_obj) ||
        desc_obj == NULL ||
        !json_object_is_type(desc_obj, json_type_string)) {
        return;
    }

    new_todo.priority = json_object_get_int(priority_obj);

    const char *desc = json_object_get_string(desc_obj);

    new_todo.desc = malloc(strlen(desc) + 1);

    if (new_todo.desc == NULL) {
        return;
    }

    strcpy(new_todo.desc, desc);

    Response *res = create_response(
        NULL,
        HTTP_OK,
        "Todos retrieved successfully"
    );

    if (res == NULL) {
        return;
    }

    send_response_data(ctx->client_fd, res);

    destroy_response(res);
}

int main(void)
{
    // Initialize Todo data.
    Todo todo1 = {
        .priority = 1,
        .desc = "wash dishes"
    };

    Todo todo2 = {
        .priority = 2,
        .desc = "clean bedroom"
    };

    Todo todos[] = {
        todo1,
        todo2
    };

    // Initialize application state.
    AppState app_state = {
        .todo_count = sizeof(todos) / sizeof(todos[0])
    };

    for (size_t i = 0; i < app_state.todo_count; i++) {
        app_state.todos[i] = todos[i];
    }

    // Initialize server.
    Server server = server_constructor(
        AF_INET,
        8080,
        SOCK_STREAM,
        0,
        10,
        INADDR_ANY,
        launch,
        &app_state
    );

    // Register routes.
    server_add_handler(
        &server,
        "/todos",
        get_todos
    );

    printf("Server listening on port 8080...\n");

    // Start server.
    server.launch(&server);

    // Cleanup after server stops.
    server_destroy(&server);

    return 0;
}