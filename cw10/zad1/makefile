CC = gcc
CFLAGS = -Wall -c

compile: clean
	@$(CC) cli.c -c
	@$(CC) $(FLAGS) cli.o -o cli
	@$(CC) srv.c -c
	@$(CC) $(FLAGS) srv.o -o srv

clean:
	@rm -f cli srv .o
