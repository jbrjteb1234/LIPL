#include <stdlib.h>
#include <stdio.h>
#include "tables.h"
#include "../ast_utility/ast.h"
#include "table_initiator.h"
#include "../ast_utility/token_scanner.h"
#include "../reducer.h"
#include "../../utility/stack.h"
#include "../ast_utility/routines.h"
#include "../ast_utility/slist_functions.h"

#define T_TYPE (*initiating_token)->token_type
#define T_VAL (*initiating_token)->token_value

#define PUSH push_token_into_ast_node(iterator, initiating_token, false);
#define PUSH_AUTO push_token_into_ast_node(iterator, initiating_token, true);
#define ADV advance_token(initiating_token);

void initiate_statement(token** initiating_token, table_iterator* iterator){
    switch(T_TYPE){
        case RESERVED_WORD: {
            //RESERVED WORD - CHECK WHICH RESERVED WORD IT IS
            switch (T_VAL.reserved_word_token_value){
                //TODO: IMPLEMENT RESERVED WORDS

                //VARIABLE DECLERATION
                case VAR: {

                    ADV //ADV TO IDENTIFIER

                    if( T_TYPE !=  IDENTIFIER) {break;}

                    PUSH //PUSH IDENTIFIER INTO NODESTACK

                    ASTNode* identifier = peek(iterator->node_stack);
                    identifier->type = LEAF_NODE;
                    identifier->value.leaf_node_value = DEC_NODE;
                    identifier->data.value_node.identifier = T_VAL.identifier_token_value;

                    iterator->table = iterator->reserved_table;
                    iterator->type = RESERVED_TABLE;

                    return;
                }
                case FUNC: {

                    ADV //ADV TO IDENTIFIER

                    if( T_TYPE !=  IDENTIFIER) {break;}

                    PUSH //PUSH IDENTIFIER INTO NODESTACK

                    //convert identifier to function node
                    ASTNode* func_dec_node = *(ASTNode**)peek(iterator->node_stack); 
                    func_dec_node->type = FUNC_NODE;
                    func_dec_node->value.func_node_value = FUNC_DEC_NODE;
                    func_dec_node->data.function_node.identifier = T_VAL.identifier_token_value;
                    func_dec_node->block_flag=true;

                    //copy in the res table manually so once the arguments have been parsed, it can return to res table to continue

                    uint32_t res_table_state = 4;
                    uint32_t res_table = jump_mask | RESERVED_TABLE;

                    push(iterator->return_stack, &res_table_state, true);
                    push(iterator->return_stack, &res_table, true);

                    iterator->table = iterator->expr_table;
                    iterator->type = EXPR_TABLE;
                    iterator->state = FCALL_EXPR_STATE;

                    return;
                }
                case IF: {

                    ASTNode* if_node = push_token_into_ast_node(iterator, NULL, false);
                    if_node->type = RES_WORD_NODE;
                    if_node->value.res_node_value = IF_NODE;
                    if_node->block_flag=true;

                    break;
                }
                case ELSE:
                    break;

                case WHILE:
                    break;

                case RETURN:
                    break;
            }

            break;
        }
        case OPERATOR:{
            break;
        }
        case DELIMITER:{

            switch (T_VAL.delimiter_token_value){
                case OPEN_BRACKET:

                    iterator->table = iterator->expr_table;
                    iterator->type = EXPR_TABLE;
                    
                    iterator->state = open_expression_parentheses(iterator, O(EXPR_OPENPAREN_STATE,0));

                    return;
            
                default:
                    break;
            }

            break;
        }
        case STRING_LITERAL:{
            iterator->table = iterator->expr_table;
            iterator->type = EXPR_TABLE;
            PUSH_AUTO
            break;
        }
        case INT_VALUE:{
            iterator->table = iterator->expr_table;
            iterator->type = EXPR_TABLE;
            PUSH_AUTO
            return;
        }
            
        case IDENTIFIER:{
            iterator->table = iterator->expr_table;
            iterator->type = EXPR_TABLE;
            iterator->state = 1;
            PUSH_AUTO
            return;
        }

        default:
            break;
    }
    perror("Unexpected token before FSM parsing");
}

/** initiates iterator with a new type of table
 * 
 */
void initiate_table(table_iterator* iterator, token** initiating_token, uint32_t state_to_save){
    printf("Initiating\n");
    if(state_to_save != N){
        uint32_t return_state = state_to_save;
        uint32_t return_table = (jump_mask | (uint32_t)iterator->type);

        push(iterator->return_stack, &return_state, true);
        push(iterator->return_stack, &return_table, true);
    }

    iterator->state = 0;
    iterator->table = NULL;
    iterator->type = NONE_TABLE;

    initiate_statement(initiating_token, iterator);

    iterator->initiated = 1;
}