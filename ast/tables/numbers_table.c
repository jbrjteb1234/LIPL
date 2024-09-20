#include "tables.h"

state_table* get_numbers_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 num/id    1 +-    2 */    3 eos   4 .
    static state_table numbers_table = {
        {N,         1,      2,      A,      N},                 //STATE 0: num
        {3,         N,      N,      N,      N},                 //STATE 1: num +-
        {4,         N,      N,      N,      N},                 //STATE 2: num */

        {N,         R(0,0), S(2),   R(0,0), },            //STATE 3: num +- num
        {N,         R(0,0), R(0,0), R(0,0), },            //STATE 4: num */ num

        {8,         N,      N,      N,      N},                 //STATE 7: num .
        {N,         R(0,0), R(0,0), R(0,0), N},                 //STATE 8: num . num
        {N,         N,      N,      N,      N},                 //STATE 9 num +- num .
        {N,         N,      N,      N,      N},                 //STATE 10 num */ num .
    };
    return &numbers_table;
}

