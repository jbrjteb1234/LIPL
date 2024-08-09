CC=gcc
CFLAGS=-I.

run: lexer_main.o main.o lexeme.o token_type.o safe_memory.o ast.o stack.o
	$(CC) $(CFLAGS) -o run lexer_main.o main.o lexeme.o token_type.o safe_memory.o ast.o stack.o

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

ast.o: ast/ast.c
	$(CC) $(CFLAGS) -c ast/ast.c

stack.o: utility/stack.c
	$(CC) $(CFLAGS) -c utility/stack.c

clean:
	rm -f *.o run