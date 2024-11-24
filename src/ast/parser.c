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

/*  TODO: 
    Create file to convert token to an index. it will also handle setting up tables if needed
    Create proper state 0 for each table to downsize table initiator
    Move away from manual token intialization, instead increase create specifier handler function
    Seperate reserved words into two categories: specifiers (var, func, global, const) which simply modify expressions to function differently and commands (like return, else, if)
    The commands will be implemented via their own state table, 
    modifiers implemented through the specifier handler function called at reduction
    Also sort out the fucking enum names
*/

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
            }case CLOSE_BLOCK: {
                           
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

            }case NOT_INITIATED: {

                initiate_table(iterator, scan_token);
                    
                break;
            }  
        }
    }

    //placeholder until AST is done
    return global_slist;
}