CC=gcc
CFLAGS=-I.

run: lexer.o main.o
	$(CC) $(CFLAGS) -o run lexer.o main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c

clean:
	rm -f *.o run