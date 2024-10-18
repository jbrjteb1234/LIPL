#include "tables.h"

#define ENTRYA 1;

state_table* get_reserved_table(void){
//      0 ID    1 NUM   2 DECL  3 EOS   4 =     5 (     6 )     7 ,     8 {     
    static state_table reserved_table = {
        {N,     N,      N,      A,      1,      N,      N,      N,      N},         //0 - DECL
        {J(0,2),J(0,2), N,      N,      N,      J(0,2), N,      N,      N},         //1 - DECL =    
        {N,     N,      N,      R(0,0), N,      N,      R(0,0), N,      N},         //2 - DECL = EXPR

        {J(0,2),J(0,2), N,      N,      N,      N,      C,      N,      N},         //3 - (

        {N,     N,      N,      A,      N,      N,      N,      N,      OB},        //4 - CONTROL STRUCTURE + (...)

        {N,     N,      N,      N,      N,      J(0,6), N,      N,      N},         //5 - IF
        {N,     N,      N,      R(2,4), N,      N,      N,      N,      R(2,4)},    //6 - IF EXPR

    };
    return &reserved_table;
}