#include "table.h"
#include "stdio.h"
state_table* get_state_table(void){
    //  R(1) = reduce binary operator (like 1+1 or 1*1)
    //  0 expr      1 +-(4)     2 */ (6)    3 eos       4 . (10)    5 = (27)    6 , (13)    7 (         8 )         9 COMP      10 {        11 }        12 FUNC     13 VAR      14 IF/WH/EI 15 ELSE     16 RETURN   17 FDEC     18 GLOBAL       19 CONST
    static state_table table = {
        {1,         N,          N,          N,          N,          N,          N,          O,          C,          N,          N,          CB,         2,          23,         15,         17,         18,         20,         40,             41},             //STATE 0: INIT

        {N,         4,          6,          A,          10,         27,         12,         N,          C,          8,          N,          N,          N,},             //STATE 1: EXPR

        {3,         N,          N,          N,          N,          N,          N,          O,          C,          N,          N,          N,          N},             //STATE 2: FUNC
        {N,         R(2),       R(2),       R(2),       N,          N,          R(2),       N,          R(2),       R(2),       N,          N,          N},             //STATE 3: FUNC EXPR

        {5,         N,          N,          N,          N,          N,          N,          O,          N,          N,          N,          N,          SV(2)},         //STATE 4: EXPR +-
        {N,         R(0),       S(6),       R(0),       S(10),      N,          R(0),       N,          R(0),       R(0),       N,          N,          N},             //STATE 5: EXPR +- EXPR

        {7,         N,          N,          N,          N,          N,          N,          O,          N,          N,          N,          N,          SV(2)},         //STATE 6: EXPR */
        {N,         R(0),       R(0),       R(0),       S(10),      N,          R(0),       N,          N,          R(0),       N,          N,          N},             //STATE 7: EXPR */ EXPR

        {9,         N,          N,          N,          N,          N,          N,          O,          N,          N,          N,          N,          SV(2)},         //STATE 8: EXPR COMP
        {N,         S(4),       S(6),       R(0),       S(10),      N,          R(0),       N,          R(0),       R(0),       N,          N,          N},             //STATE 9: EXPR COMP EXPR

        {11,        N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          SV(2)},         //STATE 10: EXPR .
        {N,         R(0),       R(0),       R(0),       R(0),       N,          R(0),       N,          R(0),       R(0),       N,          N,          N},             //STATE 11: EXPR . EXPR

        {13,        N,          N,          N,          N,          N,          N,          O,          N,          N,          N,          N,          SV(2)},         //STATE 12: EXPR ,
        {N,         S(4),       S(6),       R(1),       S(10),      N,          R(1),       N,          R(1),       R(1),       N,          N,          N},             //STATE 13: EXPR , EXPR

        {N,         N,          N,          N,          N,          N,          N,          O,          N,          N,          OB,         N,          N},             //STATE 14: BLOCK_CONTROL

        {16,        N,          N,          N,          N,          N,          N,          O,          N,          N,          N,          N,          N},             //STATE 15: CON_S 
        {N,         N,          N,          R(3),       N,          N,          N,          N,          N,          N,          R(3),       N,          N},             //STATE 16: CON_S EXPR

        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          OB,         N,          N},             //STATE 17: ELSE
        
        {19,        N,          N,          N,          N,          N,          N,          O,          N,          N,          N,          N,          N},             //STATE 18: RETURN
        {N,         N,          S(6),       R(4),       N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 19: RETURN EXPR

        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          21},            //STATE 20: FDEC 
        {22,        N,          N,          N,          N,          N,          N,          O,          C,          N,          N,          N,          N},             //STATE 21: FDEC FUNC
        {N,         N,          N,          R(5),       N,          N,          N,          N,          N,          N,          R(5),       N,          N},             //STATE 22: FDEC FUNC (EXPR)

        {24,        N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 23: VAR
        {N,         N,          N,          N,          N,          25,         N,          N,          N,          N,          N,          N,          N},             //STATE 24: VAR EXPR
        {26,        N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 25: VAR EXPR = 
        {N,         S(4),       S(6),       R(6),       S(4),       N,          S(13),      N,          N,          N,          N,          N,          N},             //STATE 26: VAR EXPR = EXPR

        {28,        N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 27: EXPR =
        {N,         S(4),       S(6),       R(0),       S(4),       N,          S(13),      N,          R(0),       N,          N,          N,          N},             //STATE 28: EXPR = EXPR              
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 29: 
        {N,         N,          N,          A,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 30:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 31: 
        {N,         N,          N,          A,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 32: 
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 33:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 34:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 35:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 36:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 37:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 38:  
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 39:

        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          23,         N,          N,          N,          20,         N,          41},            //STATE 40:  GLOBAL
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          23,         N,          N,          N,          20,         40,         N},             //STATE 41:  CONST

        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 42:
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 43:
        {N,         N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N,          N},             //STATE 44:
    };
    return &table;
}


void apply_virtual_shift(table_iterator* iterator){
    iterator->state = iterator->table[iterator->state][EXPR_INDEX];
}