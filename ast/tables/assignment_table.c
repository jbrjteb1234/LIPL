#include "tables.h"

state_table* get_assignment_table(void){
    //      0 ID    1 =     2 num   3 str   4 op    5 del       
    static state_table assignment_table = {
            {N,     1,      N,      N,      N,      A},         //STATE 0: ID
            {N,     N,      J(0,2), N,      N,      N},         //STATE 1: ID =
            {N,     N,      N,      N,      N,      R(1,0)},    //STATE 2: ID = num
            {N,     N,      N,      N,      N,      R(1,0)},    //STATE 3: ID = str
            {N,     N,      N,      N,      N,      R(1,0)},    //STATE 4: ID = ID
            
            {N,     N,      N,      N,      N,      N},         //STATE 5: spare
            {N,     N,      N,      N,      N,      N},         //STATE 6: spare
            {N,     N,      N,      N,      N,      N},         //STATE 7: spare
            {N,     N,      N,      N,      N,      N},         //STATE 8: spare
    };
    return &assignment_table;
}