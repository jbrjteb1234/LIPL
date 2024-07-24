CC=gcc
CFLAGS=-I.

run: lexer.o main.o lexeme.o
	$(CC) $(CFLAGS) -o run lexer.o main.o lexeme.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c

lexeme.o: lexeme.o
	$(CC) $(CFLAGS) -c lexeme.c

clean:
	rm -f *.o run