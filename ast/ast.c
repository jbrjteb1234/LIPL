#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"
/** Statement list - list of AST root nodes. One ast root node might hold lots of statement lists (Like a block in an if statement)
 *  The final output is a statement list - the mian routine of this source file
 */
statement_list parse(token* scan_token){

    //working list - current parent ast node where statements are being added
    statement_list working_list = (statement_list)malloc(sizeof(ASTNode*) * STATEMENT_LIST_INITIAL_SIZE);

    token* last_delimiter = NULL;
    token* current = NULL;
    bool end = false;

    unsigned char lowest_precedence = 255;

    //firstly, iterate through the whole tokenstream, stopping at each delimiter
    while(!end){
        if(scan_token->next == NULL){
            end = true;
        }

        if(scan_token->token_type == DELIMITER){
            last_delimiter = scan_token;
            //parse current working list
        }
        
        //create new, empty ASTNode and link it to token
        ASTNode* current_node = (ASTNode*)malloc(sizeof(ASTNode));
        current_node->token = scan_token;
        scan_token->ASTNode = current_node;

        int temp = 10;

        switch (scan_token->token_type) {
            case IDENTIFIER:
                // TODO: resolve identifier
                // temporary
                current_node->data.value_node.value = &temp;
                break;

            case INT_VALUE:
                // set the node's int value
                current_node->data.value_node.value = scan_token->token_value.number_token_value;
                break;

            case STRING_LITERAL:
                // set the node's string value
                current_node->data.value_node.value = scan_token->token_value.string_literal_token_value;
                break;
            
            case OPERATOR:
                // TODO: Implement operators using shunting yard algorithm
                break;

            default:
                break;
}

        scan_token = scan_token->next;
    }

    //placeholder until AST is done
    ASTNode** temp;
    return temp;
}