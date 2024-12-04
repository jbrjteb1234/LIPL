#include "table.h"
state_table* get_state_table(void){
    //  R(1) = reduce binary operator (like 1+1 or 1*1)
    //  0 var       1 id    2 +-        3 */    4 eos   5 .     6 =     7 ,         8 (         9 )         10 COMP     11 {    12 }        13 FUNC
    static state_table table = {
        {1,         2,      N,          N,      N,      N,      N,      N,          O(17,1),    N,          N,          N,          CB,     N},             //STATE 0: INITIAL

        {N,         N,      3,          4,      A,      9,      N,      14,         O(17,1),    C,          20,         N,          N,      N},             //STATE 1: var/expr
        {N,         N,      3,          4,      A,      N,      11,     14,         SNA(18),    C,          20,         N,          N,      N},             //STATE 2: ID

        {5,         7,      N,          N,      N,      N,      N,      N,          O(17,5),    N,          N,          N,          N,      N},             //STATE 3: expr +-
        {6,         8,      N,          N,      N,      N,      N,      N,          O(17,6),    N,          N,          N,          N,      N},             //STATE 4: expr */
        {N,         N,      R(0,1),     S(4),   R(0,1), S(9),   N,      R(0,1),     N,          R(0,1),     R(0,1),     N,          N,      N},             //STATE 5: expr +- var/expr
        {N,         N,      R(0,1),     R(0,1), R(0,1), S(9),   N,      R(0,1),     N,          R(0,1),     R(0,1),     N,          N,      N},             //STATE 6: expr */ var/expr
        {N,         N,      R(0,1),     S(4),   R(0,1), N,      N,      N,          SNA(18),    R(0,1),     R(0,1),     N,          N,      N},             //STATE 7: expr +- ID
        {N,         N,      R(0,1),     R(0,1), R(0,1), N,      N,      N,          SNA(18),    R(0,1),     R(0,1),     N,          N,      N},             //STATE 8: expr */ ID

        {10,        N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 9: var .
        {N,         N,      R(0,1),     R(0,1), R(0,1), N,      N,      R(0,1),     N,          R(0,1),     R(0,1),     N,          N,      N},             //STATE 10: var . var

        {10,        10,     N,          N,      N,      N,      N,      N,          O(17,10),   N,          N,          N,          N,      N},             //STATE 11: ID =
        {N,         N,      S(3),       S(4),   R(0,2), S(9),   N,      R(0,2),     N,          R(0,2),     N,          N,          N,      N},             //STATE 12: ID = var/expr
        {N,         N,      S(3),       S(4),   R(0,2), S(9),   N,      R(0,2),     SNA(18),    R(0,2),     N,          N,          N,      N},             //STATE 13: ID = ID

        {15,        16,     N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 14: expr ,          
        {N,         N,      S(3),       S(4),   R(1,1), S(9),   N,      R(1,1),     N,          R(1,1),     S(20),      N,          N,      N},             //STATE 15: expr , var/expr 
        {N,         N,      S(3),       S(4),   R(1,1), N,      S(11),  R(1,1),     SNA(18),    R(1,1),     S(20),      N,          N,      N},             //STATE 16: expr , ID

        {1,         2,      N,          N,      N,      N,      N,      N,          O(17,1),    C,          N,          N,          N,      N},             //STATE 17: (
        {N,         N,      N,          N,      N,      N,      N,      N,          O(17,19),   19,         N,          N,          N,      N},             //STATE 18: FCALL 
        {N,         N,      R(2,1),     R(2,1), R(2,1), N,      N,      R(2,1),     N,          R(2,1),     R(2,1),     R(2,1),     N,      N},             //STATE 19: FCALL (...)

        {21,        22,     N,          N,      N,      N,      N,      N,          O(17,22),   N,          N,          N,          N,      N},             //STATE 20: expr COMP
        {N,         N,      S(3),       S(4),   R(0,1), S(9),   N,      N,          N,          R(0,1),     R(0,1),     N,          N,      N},             //STATE 21: expr COMP var/expr
        {N,         N,      S(3),       S(4),   R(0,1), N,      N,      N,          SNA(18),    R(0,1),     R(0,1),     N,          N,      N},             //STATE 22: expr COMP id

        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 23: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 24: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 25: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 26: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 27: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 28: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 29: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 30: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 31: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 32: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 33: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 34: SPARE
        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 35: SPARE

        {N,         N,      N,          N,      N,      N,      N,      N,          O(17,37),   N,          N,          N,          N,      N},             //STATE 36: FUNC DEC
        {N,         N,      N,          N,      R(2,40),N,      N,      N,          N,          N,          N,          R(2,40),    N,      N},             //STATE 37: FUNC DEC, (...)

        {N,         N,      N,          N,      N,      N,      N,      N,          O(17,39),   N,          N,          N,          N,      N},             //STATE 38: CONDITIONAL                    
        {N,         N,      N,          N,      R(3,40),N,      N,      N,          N,          N,          N,          R(3,40),    N,      N},             //STATE 39: CONDITIONAL, (...)
        {N,         N,      N,          N,      A,      N,      N,      N,          N,          N,          N,          OB,         N,      N},             //STATE 40: CONTROL STRUCTURE[...]

        {N,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          OB,         N,      N},             //STATE 41: ELSE
        {S(1),      S(2),   N,          N,      A,      N,      N,      N,          O(17,43),   N,          N,          N,          N,      N},             //STATE 42: return
        {N,         N,      N,          N,      R(4,42),N,      N,      N,          N,          N,          N,          N,          N,      N},             //STATE 43: return (...)
    };
    return &table;
}