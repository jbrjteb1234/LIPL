#include "table.h"
#include "stdio.h"
state_table* get_state_table(void){
    //  R(1) = reduce binary operator (like 1+1 or 1*1)
    //  0 expr      1 +-        2 */        3 eos       4 .         5 =         6 ,         7 (         8 )         9 COMP      10 {        11 }        12 FUNC
    static state_table table = {
        {1,         N,          N,          N,          N,          N,          N,          O,          C,          N,          N,          N,          2},             //STATE 0: INIT

        {N,         4,          N,          A,          N,          N,          N,          N,          C,          N,          N,          N,          N},             //STATE 1: EXPR

        {3,         N,          N,          N,          N,          N,          N,          O,          C,          N,          N,          N,          N},             //STATE 2: FUNC
        {N,         N,          N,          R(2,1),     N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 3: FUNC (EXPR)

        {5,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          S(2)},          //STATE 4: EXPR +-
        {N,         N,          N,          R(0,1),     N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 5: EXPR +- EXPR
        
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 6: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 7: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 8: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 9: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 10: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 11: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 12: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 13: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 14: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 15: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 16: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 17: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 18: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 19: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 20: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 21: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 22: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 23: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 24: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 25: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 26:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 27:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 28:                     
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 29: 
        {N,         N,          N,          A,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 30:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 31: 
        {N,         N,          N,          A,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 32: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 33:  
    };
    return &table;
}


void apply_virtual_state(table_iterator* iterator){
    iterator->state = iterator->table[iterator->state][EXPR_INDEX];
}