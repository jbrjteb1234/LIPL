CC=gcc
CFLAGS=-I.

run: lexer_main.o main.o lexeme.o
	$(CC) $(CFLAGS) -o run lexer_main.o main.o lexeme.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

lexer_main.o: lexer/lexer_main.c
	$(CC) $(CFLAGS) -c lexer/lexer_main.c

lexeme.o: lexer/lexeme.c
	$(CC) $(CFLAGS) -c lexer/lexeme.c

clean:
	rm -f *.o run