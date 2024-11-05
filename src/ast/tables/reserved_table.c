#include "tables.h"

#define ENTRYA 1;

state_table* get_reserved_table(void){
//      0 ID    1 NUM   2 EOS   3 (     4 )     5 {          
    static state_table reserved_table = {
        {N,     N,      N,      J(0,1), N,      N},                                         //0 - CONTROL STRUCTURE
        {N,     N,      R(2,2), N,      N,      R(2,2)},                                    //1 - CONTROL STRUCTURE, (...)
        {N,     N,      A,      N,      N,      OB},                                        //2 - CONTROL STRUCTURE[...]                                        

        {N,     N,      N,      N,      N,      OB},                                        //3 - ELSE
        {N,     N,      A,      N,      N,      N},                                         //4 - return
        {J(0,4),J(0,4), R(2,4), N,      N,      N},                                         //5 - return (...)

    };
    return &reserved_table;
}