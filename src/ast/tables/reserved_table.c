#include "tables.h"

#define ENTRYA 1;

state_table* get_reserved_table(void){
//      0 ID    1 NUM   2 EOS   3 (     4 )     5 {          
    static state_table reserved_table = {
        {N,     N,      N,      J(0,1), N,      N},         //0 - FUNC DEC
        {N,     N,      R(2,4), N,      N,      R(2,4)},    //1 - FUNC DEC, (...)

        {N,     N,      N,      J(0,3), N,      N},         //2 - CONDITIONAL                    
        {N,     N,      R(3,4), N,      N,      R(3,4)},    //3 - CONDITIONAL, (...)

        {N,     N,      A,      N,      N,      OB},        //4 - CONTROL STRUCTURE[...]                                        

        {N,     N,      N,      N,      N,      OB},        //5 - ELSE

        {J(0,7),J(0,7), A,      J(0,7), N,      N},         //6 - return
        {N,     N,      R(4,6), N,      N,      N},         //7 - return (...)

    };
    return &reserved_table;
}