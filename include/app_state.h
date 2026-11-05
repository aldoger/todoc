#ifndef APP_STATE_H
#define APP_STATE_H
#include "todo.h"

#define MAX_TODOS 100 // a normal human cannot handle 100 todos to be honest

typedef struct {
    Todo todos[MAX_TODOS];
    int total_todos;
} AppState;

#endif
