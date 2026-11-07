#ifndef TODO_H
#define TODO_H

#define MAX_TODO_WORD 1024

typedef struct {
    int priority;
    const char *desc;
} Todo;

#endif