#include "table.h"
state_table* get_state_table(void){
    //  R(1) = reduce binary operator (like 1+1 or 1*1)
    //  0 var       1 id    2 +-        3 */    4 eos   5 .     6 =     7 ,         8 (         9 )         10 COMP     11 {    12 }        13 FUNC
    static state_table table = {
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 1: INIT
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 2: VAR
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 3: ID
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 4: FUNC
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 5: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 6: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 7: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 8: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 9: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 10: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 11: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 12: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 13: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 14: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 15: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 16: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 17: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 18: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 19: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 20: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 21: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 22: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 23: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 24: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 25: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 26:  
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 27:  
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 28:                     
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 29: 
        {N,         N,      N,          N,      A,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 30:  
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 31: 
        {N,         N,      N,          N,      A,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 32: 
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 33:  
    };
    return &table;
}