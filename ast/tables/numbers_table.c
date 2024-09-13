#include "tables.h"

state_table* get_numbers_table(void){
    //  R(0) = reduce binary operator (like 1+1 or 2*1)
    //  0 num/id    1 +-    2 */    3 eos/eb
    static state_table numbers_table = {
        {N,         1,      2,      A},             //STATE 0: num
        {3,         N,      N,      N},             //STATE 1: num +-
        {4,         N,      N,      N},             //STATE 2: num */

        {N,         R(0,0), 5,      R(0,0)},        //STATE 3: num +- num
        {N,         R(0,0), R(0,0), R(0,0)},        //STATE 4: num */ num

        {6,         N,      N,      N},             //STATE 5: num +- num *
        {N,         R(0,3), R(0,3), R(0,3)}         //STATE 6: num +- num * num
    };
    return &numbers_table;
}

