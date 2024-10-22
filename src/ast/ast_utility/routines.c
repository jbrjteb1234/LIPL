#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "routines.h"

uint32_t open_expression_parentheses(table_iterator* iterator, uint32_t new_state){
    uint32_t open_bracket_state_marker = C;
    uint32_t state_after_close = new_state & 0x000fffff;
    uint32_t bracket_state = (new_state & 0x0ff00000) >> open_parentheses_state_shift_count;
    new_state = bracket_state;
    printf("Opening bracket via state: %d\n", bracket_state);
    push(iterator->return_stack, &state_after_close, true);
    push(iterator->return_stack, &open_bracket_state_marker, true);
    return new_state;
}

void return_to_table(table_iterator* iterator, uint32_t new_state){
    //new table type contained within masked state on the return stack
    iterator->type = new_state & 0x0fffffff;
    
    switch(iterator->type){
        case EXPR_TABLE:
            iterator->table = iterator->expr_table;
            break;
        case RESERVED_TABLE:
            iterator->table = iterator->reserved_table;
            break;
        case NONE_TABLE:
            perror("Invalid table return");
            break;
    }
    new_state = *(uint32_t*)pop(iterator->return_stack);
    iterator->state = new_state;

    printf("Returning to table: %u on state %u\n", iterator->type, new_state);
}

void return_to_previous_state(table_iterator* iterator){
    if(iterator->return_stack->top == -1){
        perror("Tried to return to previous state without a previous state\n");
        return;
    }
    uint32_t new_state = *(uint32_t*)pop(iterator->return_stack);
    if((new_state & general_mask) == jump_mask){
        return_to_table(iterator, new_state);
    }else{
        iterator->state = new_state;
    }
}