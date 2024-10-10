CC=gcc
CFLAGS = -std=c17 -I. -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition 

run: lexer_main.o main.o lexeme.o token_dictionary.o safe_memory.o parser.o stack.o states.o reducer.o data_pool.o ast.o slist_functions.o expr_table.o table_initiator.o token_scanner.o reserved_table.o
	$(CC) $(CFLAGS) -o run lexer_main.o main.o lexeme.o token_dictionary.o safe_memory.o parser.o stack.o states.o reducer.o data_pool.o ast.o slist_functions.o expr_table.o table_initiator.o token_scanner.o reserved_table.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

lexer_main.o: lexer/lexer_main.c
	$(CC) $(CFLAGS) -c lexer/lexer_main.c

lexeme.o: lexer/lexeme.c
	$(CC) $(CFLAGS) -c lexer/lexeme.c

token_dictionary.o: lexer/token_dictionary.c
	$(CC) $(CFLAGS) -c lexer/token_dictionary.c

safe_memory.o: utility/safe_memory.c
	$(CC) $(CFLAGS) -c utility/safe_memory.c	

parser.o: ast/parser.c
	$(CC) $(CFLAGS) -c ast/parser.c

states.o: ast/states.c
	$(CC) $(CFLAGS) -c ast/states.c

stack.o: utility/stack.c
	$(CC) $(CFLAGS) -c utility/stack.c

ast.o: ast/ast_utility/ast.c
	$(CC) $(CFLAGS) -c ast/ast_utility/ast.c

slist_functions.o: ast/ast_utility/slist_functions.c
	$(CC) $(CFLAGS) -c ast/ast_utility/slist_functions.c

data_pool.o: utility/data_pool.c
	$(CC) $(CFLAGS) -c utility/data_pool.c

reducer.o: ast/reducer.c
	$(CC) $(CFLAGS) -c ast/reducer.c

expr_table.o: ast/tables/expr_table.c
	$(CC) $(CFLAGS) -c ast/tables/expr_table.c

reserved_table.o: ast/tables/reserved_table.c
	$(CC) $(CFLAGS) -c ast/tables/reserved_table.c

table_initiator.o: ast/table_initiator.c
	$(CC) $(CFLAGS) -c ast/table_initiator.c

token_scanner.o: ast/ast_utility/token_scanner.c
	$(CC) $(CFLAGS) -c ast/ast_utility/token_scanner.c

clean:
	rm -f *.o run