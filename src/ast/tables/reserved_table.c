#include "tables.h"

#define ENTRYA 1;

state_table* get_reserved_table(void){
//      0 ID    1 NUM   2 DECL  3 EOS   4 =     5 (     6 )     7 ,     8 {     
    static state_table reserved_table = {
        {J(0,4),J(0,4), N,      N,      N,      N,      C,      N,      N},         //3 - FDEC (

        {N,     N,      N,      A,      N,      N,      N,      N,      OB},        //4 - [ CONTROL STRUCTURE, (...) ]

        {N,     N,      N,      N,      N,      J(0,6), N,      N,      N},         //5 - IF
        {N,     N,      N,      R(2,4), N,      N,      N,      N,      R(2,4)},    //6 - IF EXPR

        {N,     N,      N,      N,      N,      N,      N,      N,      OB},        //7 - ELSE

    };
    return &reserved_table;
}