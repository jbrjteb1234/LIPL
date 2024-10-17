#include "tables.h"
//TODO IMPLEMENT COMMA
state_table* get_expr_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 var      1 id    2 +-    3 */     4 eos   5 .     6 =     7 ,         8 (         9 )         10 COMP
    static state_table expr_table = {
        {N,         N,      2,      3,      A,      8,      N,      13,         N,          C,          20},        //STATE 0: var/expr
        {N,         N,      2,      3,      A,      N,      10,     13,         SNJ(18),    C,          20},        //STATE 1: ID

        {4,         6,      N,      N,      N,      N,      N,      N,          O(17,4),    N,          N},         //STATE 2: expr +-
        {5,         7,      N,      N,      N,      N,      N,      N,          O(17,5),    N,          N},         //STATE 3: expr */

        {N,         N,      R(0,0), S(3),   R(0,0), S(8),   N,      R(0,0),     N,          R(0,0),     R(0,0)},    //STATE 4: expr +- var/expr
        {N,         N,      R(0,0), R(0,0), R(0,0), S(8),   N,      R(0,0),     N,          R(0,0),     R(0,0)},    //STATE 5: expr */ var/expr
        {N,         N,      R(0,0), S(3),   R(0,0), N,      N,      N,          SNJ(18),    R(0,0),     R(0,0)},    //STATE 6: expr +- ID
        {N,         N,      R(0,0), R(0,0), R(0,0), N,      N,      N,          SNJ(18),    R(0,0),     R(0,0)},    //STATE 7: expr */ ID

        {9,         N,      N,      N,      N,      N,      N,      N,          N,          N,          N},         //STATE 8: var .
        {N,         N,      R(0,0), R(0,0), R(0,0), N,      N,      R(0,0),     N,          R(0,0),     R(0,0)},    //STATE 9: var . var

        {9,         9,      N,      N,      N,      N,      N,      N,          O(17,9),    N,          N},         //STATE 10: ID =
        {N,         N,      S(2),   S(3),   R(0,1), S(8),   N,      R(0,1),     N,          R(0,1),     N},         //STATE 11: ID = var/expr
        {N,         N,      S(2),   S(3),   R(0,1), S(8),   N,      N,          SNJ(18),    N,          N},         //STATE 12: ID = ID

        {14,        15,     N,      N,      N,      N,      N,      N,          N,          N,          N},         //STATE 13: expr ,           [expr]
        {N,         N,      S(2),   S(3),   R(1,16),S(8),   N,      R(1,16),    N,          R(1,16),    N},         //STATE 14: expr , var/expr     [expr expr]
        {N,         N,      S(2),   S(3),   R(1,16),N,      S(10),  R(1,16),    SNJ(18),    R(1,16),    N},         //STATE 15: expr , ID
        {N,         N,      N,      N,      A,      N,      N,      13,         N,          C,          N},         //STATE 16: tuple


        {0,         1,      N,      N,      N,      N,      N,      N,          O(17,0),    C,          N},         //STATE 17: (
        {N,         N,      N,      N,      N,      N,      N,      N,          O(17,19),   19,         N},         //STATE 18: FCALL 
        {N,         N,      R(2,0), R(2,0), R(2,0), N,      N,      R(2,0),     N,          R(2,0),     R(2,0)},    //STATE 19: FCALL (...)

        {21,        22,     N,      N,      N,      N,      N,      N,          O(17,22),   N,          N},         //STATE 20: expr COMP
        {N,         N,      S(2),   S(3),   R(0,0), S(8),   N,      N,          N,          C,          R(0,0)},    //STATE 21: expr COMP var/expr
        {N,         N,      S(2),   S(3),   R(0,0), N,      N,      N,          SNJ(18),    C,          R(0,0)},    //STATE 22: expr COMP id


    };
    return &expr_table;
}