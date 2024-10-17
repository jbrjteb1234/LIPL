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