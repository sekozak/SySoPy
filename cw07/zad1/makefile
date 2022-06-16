CC = gcc
CFLAGS = -Wall -g

compile: clean
	@touch oven
	@touch table
	@$(CC) $(FLAGS) chief.c -o chief
	@$(CC) $(FLAGS) delivery.c -o delivery
	@$(CC) $(FLAGS) main.c -o main

clean:
	@rm -f main chief delivery oven table