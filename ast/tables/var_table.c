#include "tables.h"

state_table* get_var_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 var/id    1 +-    2 */    3 eos   4 .
    static state_table var_table = {
        {N,         1,      2,      A,      N},                 //STATE 0: var
        {3,         N,      N,      N,      N},                 //STATE 1: var +-
        {4,         N,      N,      N,      N},                 //STATE 2: var */

        {N,         R(0,0), S(2),   R(0,0), },            //STATE 3: var +- var
        {N,         R(0,0), R(0,0), R(0,0), },            //STATE 4: var */ var

        {8,         N,      N,      N,      N},                 //STATE 7: var .
        {N,         R(0,0), R(0,0), R(0,0), N},                 //STATE 8: var . var
        {N,         N,      N,      N,      N},                 //STATE 9 var +- var .
        {N,         N,      N,      N,      N},                 //STATE 10 var */ var .
    };
    return &var_table;
}

