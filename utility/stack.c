#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "safe_memory.h"

#define INITIAL_STACK_SIZE 20

stack* create_stack(size_t element_size){
    stack* new_stack = (stack*)safe_malloc(sizeof(stack));
    new_stack->top = -1;
    new_stack->max = INITIAL_STACK_SIZE;
    new_stack->element_size = element_size;
    new_stack->data = (void**)safe_malloc(element_size * INITIAL_STACK_SIZE);
    return new_stack;
}

void* pop(stack* popped_stack) {
    if (popped_stack->top == -1) {
        return NULL;  // Stack is empty
    }

    // Retrieve the element data from the stack
    void* target = (char*)popped_stack->data + (popped_stack->top * popped_stack->element_size);
    popped_stack->top--;  // Decrement the top index
    return target;  // Return the data pointer (copy)
}

void push(stack* pushed_stack, void* data_to_copy, bool dereference) {
    // Check if the stack needs to grow
    if (pushed_stack->top + 1 == (int32_t)pushed_stack->max) {
        pushed_stack->max *= 2;
        pushed_stack->data = safe_realloc(pushed_stack->data, pushed_stack->max * pushed_stack->element_size);
    }

    // Copy the element data into the stack
    pushed_stack->top++;
    if(dereference){
        void* target = (char*)pushed_stack->data + (pushed_stack->top * pushed_stack->element_size);
        safe_memcpy(target, data_to_copy, pushed_stack->element_size);  // Copy the data to the stack
    }else if(pushed_stack->element_size == sizeof(void*)){
        pushed_stack->data[++pushed_stack->top] = data_to_copy;
    }else{
        perror("Invalid combination of data to copy and dereferencing for stack\n");
    }
}

void* peek(stack* peeked_stack) {
    if (peeked_stack->top == -1) {
        return NULL;  // Stack is empty
    }

    // Return the address of the top element without removing it
    return (char*)peeked_stack->data + (peeked_stack->top * peeked_stack->element_size);
}