#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include "ast_utility/ast.h"
#include "../utility/safe_memory.h"
#include "tables/table_initiator.h"
#include "states.h"
#include "../utility/stack.h"
#include "ast_utility/slist_functions.h"
#include "ast_utility/token_scanner.h"
/** Statement list - list of AST root nodes. One ast root node might hold lots of statement lists (Like a block in an if statement)
 *  The final output is a statement list - the main routine of the program
 */

/** parses the token stream (entry point for the parser)
 *  returns statement list of all global nodes
 */
statement_list* parse(token** scan_token){

    //working list - current parent ast node where statements are being added
    
    statement_list* global_slist = create_new_slist();
    table_iterator* iterator = initialize_table_iterator(global_slist);
    stack* working_list_stack = create_stack(sizeof(statement_list*));

    shift_results result;
    bool final_token = false;

    while(!final_token){
        result = shift(iterator, scan_token);
        switch(result){
            case ERROR:
                perror("Error in parsing");
                return NULL;
            case HOLD:
                //hold same token, shift again
                break;
            case ADVANCE:
                advance_token(scan_token);
                break;
            case FINISH: {
                ASTNode* new_statement = close_iterator(iterator);
                append_to_slist(iterator->working_list, new_statement);
                printf("Completed statement\n");

                if((*scan_token)->next == NULL){
                    final_token = true;
                }else{
                    advance_token(scan_token);
                }

                break;
            }case OPEN_BLOCK: {
                ASTNode* control_block = close_iterator(iterator);
                if(control_block->block_flag == false){
                    perror("Tried to open block on non-block node\n");
                    return NULL;
                }
                append_to_slist(iterator->working_list, control_block);
                statement_list* new_block = create_new_slist();
                push(working_list_stack, iterator->working_list, false);
                control_block->block = new_block;
                iterator->working_list = new_block;

                advance_token(scan_token);

                break;
            }case NOT_INITIATED: {
                
                //CLOSING CBRACKET FUNCTION
                if((*scan_token)->token_type == DELIMITER && (*scan_token)->token_value.delimiter_token_value == CLOSE_CBRACKET){
                    if((*scan_token)->next == NULL){
                        final_token = true;
                    }else{
                        advance_token(scan_token);
                    }
                    if(working_list_stack->top == -1){
                        perror("Tried to close block without opening one\n");
                        return NULL;
                    }
                    printf("Climbing working list stack\n");
                    iterator->working_list = *(statement_list**)pop(working_list_stack);
                    break;
                }

                initiate_table(iterator, scan_token);
                    
                break;
            }  
        }
    }

    //placeholder until AST is done
    return global_slist;
}