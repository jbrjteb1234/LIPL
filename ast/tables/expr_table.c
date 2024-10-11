#include "tables.h"
//TODO IMPLEMENT COMMA
state_table* get_expr_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 expr      1 +-    2 */    3 eos   4 .     5 =     6 ,         7 (     8 )
    static state_table expr_table = {
        {N,         1,      2,      A,      5,      7,      9,          N,      C},         //STATE 0: expr
        {3,         N,      N,      N,      N,      N,      N,          O(11,3),N},         //STATE 1: expr +-
        {4,         N,      N,      N,      N,      N,      N,          O(11,4),N},         //STATE 2: expr */

        {N,         R(0,0), S(2),   R(0,0), S(5),   N,      R(0,0),     N,      R(0,0)},         //STATE 3: expr +- expr
        {N,         R(0,0), R(0,0), R(0,0), S(5),   N,      R(0,0),     N,      R(0,0)},         //STATE 4: expr */ expr

        {6,         N,      N,      N,      N,      N,      N,          N,      N},         //STATE 5: expr .
        {N,         R(0,0), R(0,0), R(0,0), N,      N,      R(0,0),     N,      R(0,0)},         //STATE 6: expr . expr

        {8,         N,      N,      N,      N,      N,      N,          O(11,8),N},         //STATE 7: expr =
        {N,         S(1),   S(2),   R(0,0), S(5),   N,      R(0,0),     N,      R(0,0)},         //STATE 8: expr = expr

        {10,        N,      N,      N,      N,      N,      N,          N,      N},         //STATE 9: expr ,           [expr]
        {N,         S(1),   S(2),   R(1,0), S(5),   S(7),   R(1,0),     N,      R(1,0)},         //STATE 10: expr , expr     [expr expr]

        {0,        N,      N,      N,      N,      N,      N,          N,      N},         //STATE 11: ( 
        {N,         N,      N,      N,      N,      N,      N,          N,      N},         //STATE 12: ( expr
        {N,         N,      N,      N,      N,      N,      N,          N,      N},         //STATE 13: ( expr )
    };
    return &expr_table;
}