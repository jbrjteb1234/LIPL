#ifndef STACK
#define STACK

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct{
    int32_t top;
    uint32_t max;
    size_t element_size;
    void** data;
}stack;

stack* create_stack(size_t element_size);

void* pop(stack* stack);

void push(stack* stack, void* data_to_copy, bool dereference);

void* peek(stack* stack);

#endif