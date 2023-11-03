#include "include/stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}


stack* initialize() {
    stack* result = (stack*)malloc(sizeof(stack));
    
    if(result == NULL) {
        printf("Unable to create stack");
        return result;
    }
    
    result->length = 0;
    result->data = NULL;

    return result;
}

void deinitialize(stack *s) {
    while (s->data != NULL) {
        linked_list* temp = s->data;
        s->data = (s->data)->prev;
        free(temp);
    }
    free(s);
}

bool is_empty(stack *s) {
    return s->length == 0;
}

bool push(stack *s, void* value) {
    printf("stack_push %i", value);
    linked_list* newItem = (linked_list*)malloc(sizeof(linked_list));

    if(newItem == NULL) {
        printf("Unable to create stack item");

        return false;
    }

    newItem->data = value;
    newItem->prev = s->data;
    s->length++;
    s->data = newItem;

    return true;
}

pop_result pop(stack *s) {
    pop_result result;
    if (is_empty(s)) {
        printf("Stack underflow\n");
        result.error = 1;
        return result;
    }

    linked_list* resultList = s->data;
    result.data = resultList->data;
    result.error = 0;
    s->data = (linked_list*)resultList->prev;
    s->length --;
    free(resultList);

    return result;
}

