compile:
	make clean

	gcc -c main.c
	gcc main.o -o main

run: compile
	make --no-print-directory test


clean:
	rm -rf *.o *.a *.so main send .txt

test:
	@./main komendy.txt
