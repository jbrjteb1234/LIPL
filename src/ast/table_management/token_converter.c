#include "token_converter.h"
#include "../ast_utility/routines.h"
#include <stdlib.h>
#include <stdio.h>

const uint32_t operator_index_lookup[20] = {
    2,2,3,3,6,10,10,10,10,10,10,5,         //var table
};

const uint32_t delimiter_index_lookup[20] = {
    4,8,9,11,12,7    //var table
};

#define VAR_INDEX 0

#define ID_INDEX 1

#define FUNC_INDEX 13

#define CREATE_NEW_AST_NODE \
    ASTNode* new_node = acquire_from_pool((iterator)->node_pool); \
    push((iterator)->node_stack, (new_node), false); \
    (new_node)->block_flag = false; \
    (new_node)->reduced = false;


/** Returns the index of the table based on the token, acquired from the tables
 * 
 */
uint32_t convert_token(table_iterator* iterator, token** current_lookahead_addr){
    
    token* current_lookahead = *current_lookahead_addr;

    switch(current_lookahead->token_type){
        case(RESERVED_WORD):

            //todo IMPLEMENT RESERVED WORDS
            switch(current_lookahead->token_value.reserved_word_token_value){
                case VAR:
                    break;
                case FUNC:
                    break;
                case IF:
                    break;
                case WHILE:
                    break;
                case ELSE:
                    break;
                case ELIF:
                    break;
                case RETURN:
                    break;
                case GLOBAL:
                    break;
                case CONST:
                    break;
                default:
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

            return VAR_INDEX;
        }

        case(INT_VALUE): {

            CREATE_NEW_AST_NODE;

            new_node->type = LEAF_NODE;
            new_node->value.leaf_node_value = INT_NODE;

            return VAR_INDEX;
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
            return ID_INDEX;
            
        }

        default:
            break;
    }
    //unrecognised symbol (erronoeus token-type)
    perror("Unrecognised token-type\n");
    return N;
}
