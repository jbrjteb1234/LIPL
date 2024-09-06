#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "safe_memory.h"

#define INITIAL_STACK_SIZE 20

stack* create_stack(int element_size){
    stack* new_stack = (stack*)safe_malloc(sizeof(stack));
    new_stack->top = -1;
    new_stack->max = INITIAL_STACK_SIZE;
    new_stack->data = (void**)safe_malloc(element_size * INITIAL_STACK_SIZE);
    return new_stack;
}

void* pop(stack* stack){
    if(stack->top == -1){
        return NULL;
    }
    return stack->data[stack->top--];
}

void push(stack* stack, void* data){
    if(stack->top == stack->max){
        stack->max *= 2;
        stack->data = (void**)safe_realloc(stack->data, stack->max);
    }
    stack->data[++stack->top] = data;
}

void* peek(stack* stack){
    return stack->data[stack->top];
}