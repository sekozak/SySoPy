compile:
	make clean
	gcc -c main.c
	gcc main.o -o main

run: compile
	@echo "/ TESTS FOR FILE 1 /" >> pomiar_zad_2.txt
	make --no-print-directory timeTest1 >> pomiar_zad_2.txt
	@echo "\n\n/ TESTS FOR FILE 2 / \n" >> pomiar_zad_2.txt
	make --no-print-directory timeTest2 >> pomiar_zad_2.txt

clean:
	rm -rf *.o *.a *.so main to.txt pomiar_zad_2.txt

timeTest1:
	@echo "\n/ TEST FOR: a /" >> pomiar_zad_2.txt
	@./main a from1.txt
	@echo "\n/ TEST FOR: t /" >> pomiar_zad_2.txt
	@./main t from1.txt

timeTest2:
	@echo "\n/ TEST FOR: a /" >> pomiar_zad_2.txt
	@./main a from2.txt
	@echo "\n/ TEST FOR: t /" >> pomiar_zad_2.txt
	@./main t from2.txt