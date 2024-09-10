#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast_utility/ast.h"
#include "../utility/safe_memory.h"
#include "states.h"
#include "../utility/stack.h"
#include "ast_utility/slist_functions.h"
/** Statement list - list of AST root nodes. One ast root node might hold lots of statement lists (Like a block in an if statement)
 *  The final output is a statement list - the main routine of the program
 */

/** Move to next token in tokenstream
 * 
 */
void advance_token(token** scan_token){
    if((*scan_token)->next != NULL){
        (*scan_token) = (*scan_token)->next;
    }
}

/** parses the token stream (entry point for the parser)
 *  returns statement list of all global nodes
 */
statement_list* parse(token* scan_token){

    //working list - current parent ast node where statements are being added
    

    table_iterator* iterator = initialize_table_iterator();
    statement_list* global_slist = create_new_slist();
    statement_list *current_working_list = global_slist;

    bool end = false;
    shift_results result;

    while(end == false){
        if(scan_token->next == NULL){
            end = true;
        }
        
        if(iterator->initiated == 0){
            initiate_table(iterator, scan_token, NONE);
        }else{
            result = shift(iterator, scan_token);
            switch(result){
            case SHIFTED:
                
                break;
            case JUMP:
                //jumped
                break;
            case REDUCED:
                //reduce
                break;
            case ERROR:
                //error
                perror("Error in parsing");
                return NULL;
            case COMPLETED: {
                ASTNode* new_statement = close_iterator(iterator);
                append_to_slist(current_working_list, new_statement);
                advance_token(&scan_token);
            }
                //completed
                break;
            }
        }

        advance_token(&scan_token);
    }

    shutdown_data_pool(iterator->progression_pool);

    //placeholder until AST is done
    return global_slist;
}