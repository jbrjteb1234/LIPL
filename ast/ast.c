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

/** Creates new AST_node
 *  Could be list of statements, an expression, a term, etc.
 */
ASTNode* new_ast_node(){
    ASTNode* new = safe_malloc(sizeof(ASTNode));
    new->statement_list_node = 0;
    return new;
}

/** Drops the working list to a sub-working list - like dropping to a block of statements in an if
 *  
 */
void drop_working_list(statement_list** current_working_list){
    statement_list* new_working_list = (statement_list*)safe_malloc(sizeof(statement_list));
    new_working_list->list = (ASTNode**)safe_malloc(sizeof(ASTNode*) * SUB_STATEMENT_LIST_INITIAL_SIZE);
    new_working_list->index=0;
    if(current_working_list != NULL){
        new_working_list->parent = (*current_working_list);
    }
}

/** returns the current working list to the working list's parent
 *  Essentially return statement, or exiting out of a block
 */
void climb_working_list(statement_list** current_working_list){
    (*current_working_list) = (*current_working_list)->parent;
}

statement_list* parse(token* scan_token){

    //working list - current parent ast node where statements are being added
    statement_list* current_working_list = (statement_list*)safe_malloc(sizeof(statement_list));
    current_working_list->list = (ASTNode**)safe_malloc(sizeof(ASTNode*)*STATEMENT_LIST_INITIAL_SIZE);
    current_working_list->index=0;

    stack* node_stack = create_stack(sizeof(ASTNode*));

    table_iterator* iterator = initialize_table_iterator();

    bool end = false;

    //LL(1) parsing method
    while(!end){
        if(scan_token->next==NULL){
            end = true;
        }
        
        if(iterator->type == NULL){
            initiate_table(iterator, scan_token);
        }

    }

    //placeholder until AST is done
    return current_working_list;
}