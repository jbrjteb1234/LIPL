#include "tables.h"

#define ENTRYA 1;

state_table* get_reserved_table(void){
//      0 ID    1 NUM   2 DECL  3 EOS   4 =     5 (     6 )     7 ,     8 {     
    static state_table reserved_table = {
        {J(0,1),J(0,1), N,      A,      N,      N,      C,      N,      OB},         //0 - FDEC

        {N,     N,      N,      A,      N,      N,      N,      N,      OB},        //1 - [ CONTROL STRUCTURE, (...) ]

        {N,     N,      N,      N,      N,      J(0,3), N,      N,      N},         //2 - IF
        {N,     N,      N,      R(2,1), N,      N,      N,      N,      R(2,1)},    //3 - IF EXPR

        {N,     N,      N,      N,      N,      N,      N,      N,      OB},        //4 - ELSE

    };
    return &reserved_table;
}