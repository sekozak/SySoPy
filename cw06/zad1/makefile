CC = gcc
CFLAGS = -Wall -g

all: compile
compile: clean
	$(CC) $(FLAGS) server.c -o server
	$(CC) $(FLAGS) client.c -o client


clean:
	rm -f server client