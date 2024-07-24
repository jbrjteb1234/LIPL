CC=gcc
CFLAGS=-I.

run: lexer.o main.o lexeme.o
	$(CC) $(CFLAGS) -o run lexer.o main.o lexeme.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer/lexer.c
	$(CC) $(CFLAGS) -c lexer/lexer.c

lexeme.o: lexer/lexeme.o
	$(CC) $(CFLAGS) -c lexer/lexeme.c

clean:
	rm -f *.o run