#include "handler.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static const char *get_status_text(StatusCode status)
{
    switch (status) {
        case HTTP_OK:
            return "OK";

        case HTTP_CREATED:
            return "Created";

        case HTTP_BAD_REQUEST:
            return "Bad Request";

        case HTTP_NOT_FOUND:
            return "Not Found";

        case HTTP_INTERNAL_SERVER_ERROR:
            return "Internal Server Error";

        default:
            return "Unknown";
    }
}

/*
 * send() might not send the entire buffer at once.
 * This function ensures that the entire buffer is transmitted.
 */
static int send_all(int client_fd, const char *buffer, size_t length)
{
    size_t total_sent = 0;

    while (total_sent < length) {
        ssize_t sent = send(
            client_fd,
            buffer + total_sent,
            length - total_sent,
            MSG_NOSIGNAL
        );

        if (sent < 0) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        if (sent == 0) {
            return -1;
        }

        total_sent += (size_t)sent;
    }

    return 0;
}

int send_response_data(int client_fd, const Response *res)
{
    if (client_fd < 0 || res == NULL) {
        return -1;
    }

    // Convert Response into a JSON object.
    struct json_object *json_obj = response_to_json_object(res);

    if (json_obj == NULL) {
        return -1;
    }

    // Serialize JSON object into a string.
    const char *json_string = json_object_to_json_string_ext(
        json_obj,
        JSON_C_TO_STRING_PLAIN
    );

    if (json_string == NULL) {
        json_object_put(json_obj);
        return -1;
    }

    size_t content_length = strlen(json_string);

    // Construct HTTP response headers.
    char headers[512];

    int header_length = snprintf(
        headers,
        sizeof(headers),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        res->status,
        get_status_text(res->status),
        content_length
    );

    if (header_length < 0 || (size_t)header_length >= sizeof(headers)) {
        json_object_put(json_obj);
        return -1;
    }

    if (send_all(client_fd, headers, (size_t)header_length) < 0) {
        json_object_put(json_obj);
        return -1;
    }

    if (send_all(client_fd, json_string, content_length) < 0) {
        json_object_put(json_obj);
        return -1;
    }

    json_object_put(json_obj);

    return 0;
}