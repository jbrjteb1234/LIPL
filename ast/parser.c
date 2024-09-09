#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast_utility/ast.h"
#include "../utility/safe_memory.h"
#include "states.h"
#include "../utility/stack.h"
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
            if(result == COMPLETED){
                //statement complete - can be added to statement list

                //shift complete
                //ASTNode* new_statement = close_iterator(iterator, current_working_list);
                //append_to_working_list(current_working_list, new_statement);
            }
        }

        advance_token(&scan_token);
    }

    shutdown_data_pool(iterator->progression_pool);

    //placeholder until AST is done
    return global_slist;
}