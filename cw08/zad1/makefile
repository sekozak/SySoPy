CC = gcc
CFLAGS = -Wall -c

compile: clean
	@$(CC) main.c -c
	@$(CC) $(FLAGS) main.o -o main -lpthread -lrt

clean:
	@rm -f main .o .txt

tests: compile
	./main 1 numbers baboon.ascii.pgm baboon1.ascii.pgm
	./main 1 numbers fractal_tree.ascii.pgm fractal_tree.ascii1.ascii.pgm
	./main 2 numbers baboon.ascii.pgm baboon2.ascii.pgm
	./main 2 numbers fractal_tree.ascii.pgm fractal_tree.ascii2.ascii.pgm
	./main 4 numbers baboon.ascii.pgm baboon4.ascii.pgm
	./main 4 numbers fractal_tree.ascii.pgm fractal_tree.ascii4.ascii.pgm
	./main 8 numbers baboon.ascii.pgm baboon8.ascii.pgm
	./main 8 numbers fractal_tree.ascii.pgm fractal_tree.ascii8.ascii.pgm
	./main 16 numbers baboon.ascii.pgm baboon16.ascii.pgm
	./main 16 numbers fractal_tree.ascii.pgm fractal_tree.ascii16.ascii.pgm