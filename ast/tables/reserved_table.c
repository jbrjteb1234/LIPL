#include "tables.h"

#define ENTRYA 1;

state_table* get_reserved_table(void){
//      0 ID    1 NUM   2 STR   3 EOS   4 =     5 (     6 )     7 ,
    static state_table reserved_table = {
        {N,     N,      N,      A,      1,      N,      N,      N},         //0 - DECL
        {N,     J(0,2), N,      N,      N,      N,      N,      N},         //1 - DECL =    
        {N,     N,      N,      R(0,0), N,      N,      N,      N},         //2 - DECL = VAR

        {N,     N,      N,      N,      N,      5,      N,      N},         //3 - FUNC DECL
        {N,     N,      N,      R(2,9), N,      N,      N,      N},         //4 - FUNC DECL (...)
        {6,     N,      N,      N,      N,      N,      4,      N},         //5 - FUNC DECL ( 
        {N,     N,      N,      N,      N,      N,      4,      7},         //6 - FUNC DECL ( ID
        {8,     N,      N,      N,      N,      N,      N,      N},         //7 - FUNC DECL ( ID ,
        {N,     N,      N,      N,      N,      N,      R(1,5), R(1,5)},    //8 - FUNC DECL ( ID , ID
        {N,     N,      N,      A,      N,      N,      N,      N},         //9 - Completed FUNC DECL
    };
    return &reserved_table;
}