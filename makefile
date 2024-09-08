CC=gcc
CFLAGS = -std=c17 -I.

run: lexer_main.o main.o lexeme.o token_type.o safe_memory.o parser.o stack.o states.o reducer.o data_pool.o ast.o
	$(CC) $(CFLAGS) -o run lexer_main.o main.o lexeme.o token_type.o safe_memory.o parser.o stack.o states.o reducer.o data_pool.o ast.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

lexer_main.o: lexer/lexer_main.c
	$(CC) $(CFLAGS) -c lexer/lexer_main.c

lexeme.o: lexer/lexeme.c
	$(CC) $(CFLAGS) -c lexer/lexeme.c

token_type.o: lexer/token_type.c
	$(CC) $(CFLAGS) -c lexer/token_type.c

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

data_pool.o: utility/data_pool.c
	$(CC) $(CFLAGS) -c utility/data_pool.c

reducer.o: ast/reducer.c
	$(CC) $(CFLAGS) -c ast/reducer.c

clean:
	rm -f *.o run