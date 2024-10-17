#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include "ast_utility/ast.h"
#include "../utility/safe_memory.h"
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
    

    table_iterator* iterator = initialize_table_iterator();
    statement_list* global_slist = create_new_slist();
    statement_list *current_working_list = global_slist;

    shift_results result;

    while(true){
        
        if(iterator->initiated == 0){
            initiate_table(iterator, scan_token, NONE_TABLE);
        }else{
            advance_token(scan_token);
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
                break;
            }
            }
        }
        if((*scan_token)->next == NULL){
            break;
        }
    }

    shutdown_data_pool(iterator->progression_pool);

    //placeholder until AST is done
    return global_slist;
}