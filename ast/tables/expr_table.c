#include "tables.h"
//TODO IMPLEMENT COMMA
state_table* get_expr_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 var      1 id    2 +-    3 */     4 eos   5 .     6 =     7 ,         8 (         9 )
    static state_table expr_table = {
        {N,         N,      2,      3,      A,      6,      N,      10,         N,          C},         //STATE 0: var
        {N,         N,      2,      3,      A,      N,      8,      10,         O(15,16),   C},         //STATE 1: ID

        {4,         3,      N,      N,      N,      N,      N,      N,          O(14,4),    N},         //STATE 2: expr +-
        {5,         4,      N,      N,      N,      N,      N,      N,          O(14,5),    N},         //STATE 3: expr */

        {N,         N,      R(0,0), S(3),   R(0,0), S(6),   N,      R(0,0),     N,          R(0,0)},    //STATE 4: expr +- expr
        {N,         N,      R(0,0), R(0,0), R(0,0), S(6),   N,      R(0,0),     N,          R(0,0)},    //STATE 5: expr */ expr

        {7,         N,      N,      N,      N,      N,      N,      N,          N,          N},         //STATE 6: var .
        {N,         N,      R(0,0), R(0,0), R(0,0), N,      N,      R(0,0),     N,          R(0,0)},    //STATE 7: var . var

        {9,         9,      N,      N,      N,      N,      N,      N,          O(14,9),    N},         //STATE 8: ID =
        {N,         N,      S(2),   S(3),   R(0,1), S(6),   N,      R(0,1),     N,          R(0,1)},    //STATE 9: ID = expr

        {11,        12,     N,      N,      N,      N,      N,      N,          N,          N},         //STATE 10: expr ,           [expr]
        {N,         N,      S(2),   S(3),   R(1,13),S(6),   N,      R(1,13),    N,          R(1,13)},   //STATE 11: expr , var     [expr expr]
        {N,         N,      S(2),   S(3),   R(1,13),N,      S(8),   R(1,13),    N,          R(1,13)},   //STATE 12: expr , ID
        {N,         N,      N,      N,      A,      N,      N,      10,         N,          C},         //STATE 13: tuple


        {0,         1,      N,      N,      N,      N,      N,      N,          N,          N},         //STATE 14: (
        {0,         1,      N,      N,      N,      N,      N,      N,          N,          C},         //STATE 15: FCALL (
        {N,         N,      R(2,0), R(2,0), R(2,0), N,      N,      R(2,0),     N,          N},         //STATE 16: FCALL (...)
    };
    return &expr_table;
}