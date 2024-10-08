#include "tables.h"

state_table* get_var_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 var/id    1 +-    2 */    3 eos   4 .
    static state_table var_table = {
        {N,         1,      2,      A,      5},                 //STATE 0: var
        {3,         N,      N,      N,      N},                 //STATE 1: var +-
        {4,         N,      N,      N,      N},                 //STATE 2: var */

        {N,         R(0,0), S(2),   R(0,0), S(5)},            //STATE 3: var +- var
        {N,         R(0,0), R(0,0), R(0,0), S(5)},            //STATE 4: var */ var

        {6,         N,      N,      N,      N},                 //STATE 5: var .
        {N,         R(0,0), R(0,0), R(0,0), N},                 //STATE 6: var . var
    };
    return &var_table;
}

