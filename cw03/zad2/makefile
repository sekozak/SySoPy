compile:
	make clean
	gcc -c main.c
	gcc main.o -o main

run: compile
	@echo "/ TESTS / \n" >> w.txt
	make --no-print-directory timeTest1 >> w.txt
	make --no-print-directory timeTest2 >> w.txt
	make --no-print-directory timeTest3 >> w.txt
	make --no-print-directory timeTest4 >> w.txt
	make --no-print-directory timeTest5 >> w.txt
	make --no-print-directory timeTest6 >> w.txt

clean:
	rm -rf *.o *.a *.so main *.txt


timeTest1:
	@echo "\n/ TEST FOR: 0.2 1000 /" >> w.txt
	@./main 0.2 1000

timeTest2:
	@echo "\n/ TEST FOR: 0.1 2000 /" >> w.txt
	@./main 0.1 2000

timeTest3:
	@echo "\n/ TEST FOR: 0.002 1000 /" >> w.txt
	@./main 0.002 1000

timeTest4:
	@echo "\n/ TEST FOR: 0.001 2000 /" >> w.txt
	@./main 0.001 2000

timeTest5:
	@echo "\n/ TEST FOR: 0.00002 1000 /" >> w.txt
	@./main 0.00002 1000

timeTest6:
	@echo "\n/ TEST FOR: 0.00001 2000 /" >> w.txt
	@./main 0.00001 2000