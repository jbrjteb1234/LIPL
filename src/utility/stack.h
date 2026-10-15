#ifndef LIPL_UTILITY_STACK_H
#define LIPL_UTILITY_STACK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Growable stack of fixed-size elements, stored by value. */
typedef struct stack{
    int32_t top;            /* index of the top element, -1 when empty */
    uint32_t max;           /* capacity, in elements */
    size_t element_size;
    unsigned char* data;
}stack;

stack* create_stack(size_t element_size);

void destroy_stack(stack* stack);

/* Removes the top element and returns a pointer to it, or NULL if the stack is
   empty. The pointer is only valid until the next push. */
void* pop(stack* stack);

/* dereference = true:  copies element_size bytes from data_to_copy.
   dereference = false: stores the pointer data_to_copy itself
                        (element_size must be sizeof(void*)). */
void push(stack* stack, void* data_to_copy, bool dereference);

/* Returns a pointer to the top element without removing it, or NULL if empty. */
void* peek(stack* stack);

bool stack_is_empty(const stack* stack);

#endif /* LIPL_UTILITY_STACK_H */
