compile:
	make clean
	gcc -c main.c
	gcc main.o -o main

run: compile
	@echo "/ TESTS / \n" >> pomiar_zad_1.txt
	make --no-print-directory timeTest1 >> pomiar_zad_1.txt
	make --no-print-directory timeTest2 >> pomiar_zad_1.txt

clean:
	rm -rf *.o *.a *.so main *to.txt pomiar_zad_1.txt

timeTest1:
	@echo "\n/ TEST FOR small file /" >> pomiar_zad_1.txt
	@./main small.txt small_to.txt

timeTest2:
	@echo "\n/ TEST FOR big file/" >> pomiar_zad_1.txt
	@./main big.txt big_to.txt