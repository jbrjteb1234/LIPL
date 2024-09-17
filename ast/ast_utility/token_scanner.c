#include <stdio.h>
#include <stdlib.h>
#include "token_scanner.h"

/** Move to next token in tokenstream
 * 
 */
void advance_token(token** scan_token){
    if((*scan_token)->next != NULL){
        (*scan_token) = (*scan_token)->next;
    }else{
        //perror("Tried to advance past end of tokenstream\n");
    }
}