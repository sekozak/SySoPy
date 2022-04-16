compile:
	make clean
	gcc -c main.c
	gcc -c main2.c
	gcc -c exec.c
	gcc main.o -o main
	gcc main2.o -o main2
	gcc exec.o -o exec

run: compile
	make --no-print-directory testForFork
	@echo "\n\n" >> raport2.txt
	make --no-print-directory testForExec

clean:
	rm -rf *.o *.a *.so main

testForFork:
	@echo "----- Test for FORK -----\n"
	@echo "--- Step by step in ignore ---"
	@./main ignore
	@echo "\n--- Step by step in handle ---"
	@./main handle
	@echo "\n--- Step by step in mask ---"
	@./main mask
	@echo "\n--- Step by step in pending ---"
	@./main pending

testForExec:
	@echo "----- Test for EXEC -----\n"
	@echo "--- Step by step in ignore ---"
	@./main2 ignore 
	@echo "\n--- Step by step in mask ---"
	@./main2 mask
	@echo "\n--- Step by step in pending ---"
	@./main2 pending 