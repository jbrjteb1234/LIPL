#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"
#include "safe_memory.h"

#define INITIAL_STACK_SIZE 20u

stack* create_stack(size_t element_size){
    stack* new_stack = (stack*)safe_malloc(sizeof(stack));
    new_stack->top = -1;
    new_stack->max = INITIAL_STACK_SIZE;
    new_stack->element_size = element_size;
    new_stack->data = (unsigned char*)safe_malloc(element_size * INITIAL_STACK_SIZE);
    return new_stack;
}

void destroy_stack(stack* destroyed_stack){
    if (!destroyed_stack) return;
    safe_free((void**)&destroyed_stack->data);
    safe_free((void**)&destroyed_stack);
}

void* pop(stack* popped_stack) {
    if (popped_stack->top == -1) {
        return NULL;  // Stack is empty
    }

    // Retrieve the element data from the stack
    void* target = popped_stack->data + (size_t)popped_stack->top * popped_stack->element_size;
    popped_stack->top--;  // Decrement the top index
    return target;
}

void push(stack* pushed_stack, void* data_to_copy, bool dereference) {
    if (!dereference && pushed_stack->element_size != sizeof(void*)) {
        fprintf(stderr, "fatal: pushed a pointer onto a stack of %u-byte elements\n",
                (unsigned)pushed_stack->element_size);
        exit(EXIT_FAILURE);
    }

    // Check if the stack needs to grow
    if ((uint32_t)(pushed_stack->top + 1) == pushed_stack->max) {
        if (pushed_stack->max >= (uint32_t)INT32_MAX / 2u) {
            fprintf(stderr, "fatal: stack overflow\n");
            exit(EXIT_FAILURE);
        }
        pushed_stack->max *= 2u;
        pushed_stack->data = safe_realloc(pushed_stack->data, (size_t)pushed_stack->max * pushed_stack->element_size);
    }

    // Copy the element data into the stack
    pushed_stack->top++;
    unsigned char* target = pushed_stack->data + (size_t)pushed_stack->top * pushed_stack->element_size;
    if (dereference) {
        safe_memcpy(target, data_to_copy, pushed_stack->element_size);
    } else {
        memcpy(target, &data_to_copy, sizeof(void*));
    }
}

void* peek(stack* peeked_stack) {
    if (peeked_stack->top == -1) {
        return NULL;  // Stack is empty
    }

    // Return the address of the top element without removing it
    return peeked_stack->data + (size_t)peeked_stack->top * peeked_stack->element_size;
}

bool stack_is_empty(const stack* checked_stack) {
    return checked_stack->top == -1;
}
