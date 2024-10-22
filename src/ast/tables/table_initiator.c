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
                ASTNode* current_node;
                case VAR: {

                    ADV //ADV TO IDENTIFIER

                    if( T_TYPE !=  IDENTIFIER) {break;}

                    PUSH //PUSH IDENTIFIER INTO NODESTACK

                    ASTNode* current_node = peek(iterator->node_stack);
                    current_node->type = LEAF_NODE;
                    current_node->value.leaf_node_value = DEC_NODE;
                    current_node->data.value_node.identifier = T_VAL.identifier_token_value;

                    iterator->table = iterator->reserved_table;
                    iterator->type = RESERVED_TABLE;

                    return;
                }
                case FUNC: {

                    ADV //ADV TO IDENTIFIER

                    if( T_TYPE !=  IDENTIFIER) {break;}

                    //convert identifier to function node
                    current_node = PUSH //PUSH IDENTIFIER IN TO NODE STACK
                    current_node->type = FUNC_NODE;
                    current_node->value.func_node_value = FUNC_DEC_NODE;
                    current_node->data.function_node.identifier = T_VAL.identifier_token_value;
                    current_node->block_flag=true;

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
                case IF:
                case WHILE:

                    current_node = PUSH
                    current_node->type = RES_WORD_NODE;
                    
                    if(T_VAL.reserved_word_token_value == IF){
                        current_node->value.res_node_value = IF_NODE;
                    }else{
                        current_node->value.res_node_value = WHILE_NODE;
                    }
                    
                    current_node->block_flag=true;

                    iterator->table = iterator->reserved_table;
                    iterator->type = RESERVED_TABLE;
                    iterator->state = 5;

                    return;
        
                case ELSE:
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
bool initiate_table(table_iterator* iterator, token** initiating_token, uint32_t state_to_save){
    printf("Initiating\n");
    if(state_to_save != N){
        uint32_t return_state = state_to_save;
        uint32_t return_table = (jump_mask | (uint32_t)iterator->type);

        push(iterator->return_stack, &return_state, true);
        push(iterator->return_stack, &return_table, true);
    }

    if((*initiating_token)->token_type == DELIMITER && (*initiating_token)->token_value.delimiter_token_value == CLOSE_CBRACKET){
        advance_token(initiating_token);
        return true;
    }

    iterator->state = 0;
    iterator->table = NULL;
    iterator->type = NONE_TABLE;

    initiate_statement(initiating_token, iterator);

    iterator->initiated = 1;

    advance_token(initiating_token);

    return false;
}