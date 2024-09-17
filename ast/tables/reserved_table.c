#include "tables.h"

state_table* get_reserved_table(void){
        //ID    NUM     STR     VAR     FUNC    IF      ELSE    WHILE      
    static state_table reserved_table = {
        {N,     N,      N,      N,      N,      N,      N,      N},//0 - VAR ID =
        {N,     N,      N,      N,      N,      N,      N,      N},//1 - VAR ID = NUM
        
    };
    return &reserved_table;
}