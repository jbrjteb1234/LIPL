#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "safe_memory.h"

#define INITIAL_STACK_SIZE 20

stack* create_stack(size_t element_size){
    stack* new_stack = (stack*)safe_malloc(sizeof(stack));
    new_stack->top = -1;
    new_stack->max = INITIAL_STACK_SIZE;
    new_stack->data = (void**)safe_malloc(element_size * INITIAL_STACK_SIZE);
    return new_stack;
}

void* pop(stack* popped_stack){
    if(popped_stack->top == -1){
        return NULL;
    }
    return popped_stack->data[popped_stack->top--];
}

void push(stack* pushed_stack, void* data){
    if((uint32_t)(pushed_stack->top + 1) == pushed_stack->max){
        pushed_stack->max *= 2;
        pushed_stack->data = (void**)safe_realloc(pushed_stack->data, (size_t) pushed_stack->max);
    }
    pushed_stack->data[++pushed_stack->top] = data;
}

void* peek(stack* peeked_stack){
    return peeked_stack->data[peeked_stack->top];
}