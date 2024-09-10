#ifndef STACK
#define STACK

#include <stdint.h>

typedef struct{
    int32_t top;
    uint32_t max;
    void** data;
}stack;

stack* create_stack(size_t element_size);

void* pop(stack* stack);

void push(stack* stack, void* data);

void* peek(stack* stack);

#endif