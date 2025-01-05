#include "token_converter.h"
#include "../ast_utility/routines.h"
#include <stdlib.h>
#include <stdio.h>

const uint32_t operator_index_lookup[20] = {
    1,1,2,2,5,9,9,9,9,9,9,4,         //var table
};

const uint32_t delimiter_index_lookup[20] = {
    3,7,8,10,11,6    //var table
};

#define CREATE_NEW_AST_NODE \
    ASTNode* new_node = acquire_from_pool((iterator)->node_pool); \
    (new_node)->block_flag = false; \
    (new_node)->reduced = false; \
    iterator->new_node_buffer = new_node; \
    iterator->new_node_buffer_set_flag = true

/** Returns the index of the table based on the token, acquired from the tables
 * 
 */
token_conversion_results convert_token(table_iterator* iterator, token** current_lookahead_addr){
    
    token* current_lookahead = *current_lookahead_addr;
    token_types next_type = (*current_lookahead_addr)->token_type;
    iterator->new_node_buffer_set_flag = false;

    switch(next_type){
        case(RESERVED_WORD):

            //todo IMPLEMENT RESERVED WORDS
            switch(current_lookahead->token_value.reserved_word_token_value){
                case VAR:
                case FUNC:
                case GLOBAL:
                case CONST:

                    set_specifiers(iterator, current_lookahead->token_value.reserved_word_token_value);
                    return C_ADVANCE;

                case IF: {

                    CREATE_NEW_AST_NODE;
                    new_node->type = CONDITIONAL_BLOCK_NODE;
                    new_node->value.conditional_block_node_value = IF_NODE;
                    new_node->block_flag = true;
                    new_node->data.conditional_block_node.alternate = NULL;

                    return C_ADVANCE;
                    
                }case WHILE: {

                    CREATE_NEW_AST_NODE;
                    new_node->type = CONDITIONAL_BLOCK_NODE;
                    new_node->value.conditional_block_node_value = WHILE_NODE;
                    new_node->block_flag = true;
                    new_node->data.conditional_block_node.alternate = NULL;
                    
                    return C_ADVANCE;
                
                }case ELSE: {

                    CREATE_NEW_AST_NODE;
                    new_node->type = CONDITIONAL_BLOCK_NODE;
                    new_node->value.conditional_block_node_value = ELSE_NODE;
                    new_node->block_flag = true;
                    new_node->data.conditional_block_node.alternate = NULL;

                    return C_ADVANCE_ADD_ALTERNATE;

                }case ELIF: {

                    CREATE_NEW_AST_NODE;
                    new_node->type = CONDITIONAL_BLOCK_NODE;
                    new_node->value.conditional_block_node_value = ELIF_NODE;
                    new_node->block_flag = true;
                    new_node->data.conditional_block_node.alternate = NULL;

                    return C_ADVANCE_ADD_ALTERNATE;

                }case RETURN: {

                    CREATE_NEW_AST_NODE;
                    new_node->type = RES_WORD_NODE;
                    new_node->value.reserved_word_value = RETURN_NODE;

                    return C_ADVANCE;

                }default:
                    break;
            }

            return N;

        case(OPERATOR):

            CREATE_NEW_AST_NODE;

            new_node->type = BINARY_OP_NODE;
            new_node->value.binary_op_node_value = (binary_op_node)current_lookahead->token_value.operator_token_value;

            return operator_index_lookup[current_lookahead->token_value.operator_token_value];

            break;
        case(DELIMITER):

            return delimiter_index_lookup[current_lookahead->token_value.delimiter_token_value];

            break;

        case(STRING_LITERAL): {

            CREATE_NEW_AST_NODE;

            new_node->type = LEAF_NODE;
            new_node->value.leaf_node_value = STR_NODE;

            return EXPR_INDEX;
        }

        case(INT_VALUE): {

            CREATE_NEW_AST_NODE;

            new_node->type = LEAF_NODE;
            new_node->value.leaf_node_value = INT_NODE;

            return EXPR_INDEX;
        }

        case(IDENTIFIER): {

            CREATE_NEW_AST_NODE;

            token* next = lookahead(current_lookahead_addr);

            if( next->token_type == DELIMITER && next->token_value.delimiter_token_value == OPEN_BRACKET ){
                new_node->type = FUNC_NODE;
                if(read_specifiers_field(new_node, FUNC_NODE)){
                    new_node->value.func_node_value = FUNC_DEC_NODE;
                    return N;
                }
                new_node->value.func_node_value = FUNC_CALL_NODE;
                return FUNC_INDEX;
            }
            new_node->type = LEAF_NODE;
            new_node->value.leaf_node_value = ID_NODE;
            return EXPR_INDEX;
            
        }

        default:
            break;
    }
    //unrecognised symbol (erronoeus token-type)
    perror("Unrecognised token-type\n");
    return C_ERROR;
}
