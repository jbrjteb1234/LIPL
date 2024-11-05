#include "tables.h"
state_table* get_expr_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 var      1 id    2 +-    3 */     4 eos   5 .     6 =         7 ,         8 (         9 )         10 COMP     11 {
    static state_table expr_table = {
        {N,         N,      2,          3,      A,      8,      N,      13,         O(16,0),    C,          19,         OB},            //STATE 0: var/expr
        {N,         N,      2,          3,      A,      N,      10,     13,         SNA(17),    C,          19,         N},             //STATE 1: ID
    
        {4,         6,      N,          N,      N,      N,      N,      N,          O(16,4),    N,          N,          N},             //STATE 2: expr +-
        {5,         7,      N,          N,      N,      N,      N,      N,          O(16,5),    N,          N,          N},             //STATE 3: expr */
    
        {N,         N,      R(0,0),     S(3),   R(0,0), S(8),   N,      R(0,0),     N,          R(0,0),     R(0,0),     N},             //STATE 4: expr +- var/expr
        {N,         N,      R(0,0),     R(0,0), R(0,0), S(8),   N,      R(0,0),     N,          R(0,0),     R(0,0),     N},             //STATE 5: expr */ var/expr
        {N,         N,      R(0,0),     S(3),   R(0,0), N,      N,      N,          SNA(17),    R(0,0),     R(0,0),     N},             //STATE 6: expr +- ID
        {N,         N,      R(0,0),     R(0,0), R(0,0), N,      N,      N,          SNA(17),    R(0,0),     R(0,0),     N},             //STATE 7: expr */ ID
    
        {9,         N,      N,          N,      N,      N,      N,      N,          N,          N,          N,          N},             //STATE 8: var .
        {N,         N,      R(0,0),     R(0,0), R(0,0), N,      N,      R(0,0),     N,          R(0,0),     R(0,0),     N},             //STATE 9: var . var
    
        {9,         9,      N,          N,      N,      N,      N,      N,          O(16,9),    N,          N,          N},             //STATE 10: ID =
        {N,         N,      S(2),       S(3),   R(0,1), S(8),   N,      R(0,1),     N,          R(0,1),     N,          N},             //STATE 11: ID = var/expr
        {N,         N,      S(2),       S(3),   R(0,1), S(8),   N,      R(0,1),     SNA(17),    R(0,1),     N,          N},             //STATE 12: ID = ID
    
        {14,        15,     N,          N,      N,      N,      N,      N,          N,          N,          N,          N},             //STATE 13: expr ,          
        {N,         N,      S(2),       S(3),   R(1,0), S(8),   N,      R(1,0),     N,          R(1,0),     S(19),      N},             //STATE 14: expr , var/expr 
        {N,         N,      S(2),       S(3),   R(1,0), N,      S(10),  R(1,0),     SNA(17),    R(1,0),     S(19),      N},             //STATE 15: expr , ID
    
    
        {0,         1,      N,          N,      N,      N,      N,      N,          O(16,0),    C,          N,          N},             //STATE 16: (
        {N,         N,      N,          N,      N,      N,      N,      N,          O(16,18),   18,         N,          N},             //STATE 17: FCALL 
        {N,         N,      R(2,0),     R(2,0), R(2,0), N,      N,      R(2,0),     N,          R(2,0),     R(2,0),     R(2,0)},        //STATE 18: FCALL (...)

        {20,        21,     N,          N,      N,      N,      N,      N,          O(16,21),   N,          N,          N},             //STATE 19: expr COMP
        {N,         N,      S(2),       S(3),   R(0,0), S(8),   N,      N,          N,          R(0,0),     R(0,0),     N},             //STATE 20: expr COMP var/expr
        {N,         N,      S(2),       S(3),   R(0,0), N,      N,      N,          SNA(17),    R(0,0),     R(0,0),     N},             //STATE 21: expr COMP id
    };
    return &expr_table;
}