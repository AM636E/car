#ifndef STACK
#define STACK
#include <stdbool.h>
#define MAX_SIZE 300

typedef struct linked_list
{
    struct linked_list *prev;
    void *data;
} linked_list;

typedef struct
{
    void *data;
    bool error;
} pop_result;

typedef struct
{
    linked_list *data;
    int length;
} stack;

int clamp(int value, int min, int max);

stack *initialize();
void deinitialize(stack *s);
bool is_empty(stack *s);

bool is_full(stack *s);
bool push(stack *s, void *value);

pop_result pop(stack *s);

#endif