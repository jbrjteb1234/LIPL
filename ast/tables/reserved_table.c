#include "tables.h"

#define ENTRYA 1;

state_table* get_reserved_table(void){
//      0 ID    1 NUM   2 STR   3 DEL   4 =
    static state_table reserved_table = {
        {N,     N,      N,      A,      1,      N,      N,      N}, //0 - DECL
        {N,     J(0,2), N,      N,      N,      N,      N,      N}, //1 - DECL =    
        {N,     N,      N,      R(0,0), N,      N,      N,      N}, //2 - DECL = VAR

        {N,     N,      N,      N,      N,      N,      N,      N}, //3 - FUNC DECL (
        {N,     N,      N,      N,      N,      N,      N,      N}, // 
        {N,     N,      N,      N,      N,      N,      N,      N}, //
    };
    return &reserved_table;
}