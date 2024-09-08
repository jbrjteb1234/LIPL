#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"
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

void append_to_working_list(statement_list* current_list, ASTNode* new_statement){
    current_list->list[current_list->index] = new_statement;
    ++current_list->index;
}

/** parses the token stream (entry point for the parser)
 *  returns statement list of all global nodes
 */
statement_list* parse(token* scan_token){

    //working list - current parent ast node where statements are being added
    statement_list* current_working_list = (statement_list*)safe_malloc(sizeof(statement_list));
    current_working_list->list = (ASTNode**)safe_malloc(sizeof(ASTNode*)*STATEMENT_LIST_INITIAL_SIZE);
    current_working_list->index=0;

    table_iterator* iterator = initialize_table_iterator();

    bool end = false;

    while(end == false){
        if(scan_token->next == NULL){
            end = true;
        }
        
        if(iterator->initiated == 0){
            initiate_table(iterator, scan_token);
        }else{
            if(shift(iterator, scan_token)){
                //shift complete
                ASTNode* new_statement = close_iterator(iterator, current_working_list);
                append_to_working_list(current_working_list, new_statement);
            }
        }

        advance_token(&scan_token);
    }

    shutdown_data_pool(iterator->progression_pool);

    //placeholder until AST is done
    return current_working_list;
}