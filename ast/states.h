#ifndef STATES
#define STATES

#define N -1
#define A -2
#define R(A) (-(A)-3)

typedef enum {
    MATHS_TABLE
} action_tables;

//      R(0) = reduce binary operator (like 1+1 or 2*1)
//      num     +-      */      del      
int numbers_table[][4] = {
        {N,     1,      2,      A},         //STATE 0: num
        {3,     N,      N,      N},         //STATE 1: num +-
        {4,     N,      N,      N},         //STATE 2: num */

        {N,     R(0),   5,      R(0)},      //STATE 3: num +- num
        {N,     R(0),   R(0),   R(0)},      //STATE 4: num */ num

        {6,     N,      N,      N},         //STATE 5: num +- num *
        {N,     R(0),   R(0),   4}          //STATE 6: num +- num * num
};

//      ID      =       num     str     op      del       
int decl_table[][6] = {
        {N,     2,      N,      N,      N,      N},         //STATE 0: ID
        {N,     N,      N,      N,      N,      N},         //STATE 1: ID =
        {N,     N,      N,      N,      N,      N},         //STATE 2: ID = num
        {N,     N,      N,      N,      N,      N},         //STATE 3: ID = str
        {N,     N,      N,      N,      N,      N},         //STATE 4: ID = ID
        
        {N,     N,      N,      N,      N,      N},         //STATE 5: ID = num (op)
        {N,     N,      N,      N,      N,      N},         //STATE 6: ID = str (op)
        {N,     N,      N,      N,      N,      N}          //STATE 7: ID = ID (op)
};

typedef struct{
    int** table;
    int state;
} table_iterator;

void advance_state (table_iterator*);

#endif