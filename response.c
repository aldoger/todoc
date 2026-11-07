#include "response.h"

#include <stdlib.h>
#include <string.h>

Response *create_response(
    struct json_object *obj,
    StatusCode status,
    const char *message
) {
    Response *response = malloc(sizeof(Response));

    if (response == NULL) {
        return NULL;
    }

    response->obj = obj ? json_object_get(obj) : NULL;
    response->status = status;
    response->message = NULL;

    if (message != NULL) {
        size_t message_length = strlen(message) + 1;

        response->message = malloc(message_length);

        if (response->message == NULL) {
            if (response->obj != NULL) {
                json_object_put(response->obj);
            }

            free(response);
            return NULL;
        }

        memcpy(response->message, message, message_length);
    }

    return response;
}

struct json_object *response_to_json_object(const Response *response) {
    if (response == NULL) {
        return NULL;
    }

    struct json_object *root = json_object_new_object();

    if (root == NULL) {
        return NULL;
    }

    struct json_object *status_obj =
        json_object_new_int(response->status);

    if (status_obj == NULL ||
        json_object_object_add(root, "status", status_obj) != 0) {
        if (status_obj != NULL) {
            json_object_put(status_obj);
        }

        json_object_put(root);
        return NULL;
    }

    struct json_object *message_obj = response->message
        ? json_object_new_string(response->message)
        : NULL;

    if ((response->message != NULL && message_obj == NULL) ||
        json_object_object_add(root, "message", message_obj) != 0) {
        if (message_obj != NULL) {
            json_object_put(message_obj);
        }

        json_object_put(root);
        return NULL;
    }

    struct json_object *data_obj = response->obj
        ? json_object_get(response->obj)
        : NULL;

    if (json_object_object_add(root, "data", data_obj) != 0) {
        if (data_obj != NULL) {
            json_object_put(data_obj);
        }

        json_object_put(root);
        return NULL;
    }

    return root;
}

void destroy_response(Response *response) {
    if (response == NULL) {
        return;
    }

    if (response->obj != NULL) {
        json_object_put(response->obj);
    }

    free(response->message);
    free(response);
}