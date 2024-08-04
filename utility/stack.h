#ifndef STACK
#define STACK

typedef struct{
    int top;
    int max;
    void** data;
}stack;

stack* create_stack(int element_size);

void* pop(stack* stack);

void push(stack* stack, void* data);

void* peek(stack* stack);

#endif