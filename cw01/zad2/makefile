statyczna:
	make clean
	gcc -c main.c
	gcc -c bibl.c
	ar rcs libbibl.a bibl.o
	gcc main.o libbibl.a -o main

clean:
	rm -rf *.o *.a *.so main raport2.txt

wspoldzielona:
	make clean
	gcc -fPIC -c bibl.c
	gcc -c main.c
	gcc -shared bibl.o -o libbibl.so
	gcc main.o -L. -lbibl -Wl,-rpath=. -o main

test_static: statyczna
	@echo "TESTS FOR STATYCZNA" >> raport2.txt
	make --no-print-directory test_1 >> raport2.txt
	make --no-print-directory test_2 >> raport2.txt
	make --no-print-directory test_3 >> raport2.txt

test_wspol: wspoldzielona
	@echo "TESTS FOR WSPOLDZEILONA\n" >> raport2.txt
	make  --no-print-directory test_1 >> raport2.txt
	make --no-print-directory test_2 >> raport2.txt
	make --no-print-directory test_3 >> raport2.txt

test_1:
	@echo "\TEST 1 \n" >> raport2.txt
	@./main ./create_table 7 ./wc_files medium.txt ./wc_files text1.txt ./wc_files text2.txt \
	 ./wc_files large.txt ./wc_files small.txt

test_2:
	@echo "\TEST 2 \n" >> raport2.txt
	@./main ./create_table 5 ./wc_files medium.txt ./wc_files text1.txt ./remove_block 1 ./wc_files small.txt

test_3:
	@echo "\TESTS 3 \n" >> raport2.txt
	@./main ./create_table 3 ./wc_files medium.txt ./remove_block 0 ./wc_files large.txt ./remove_block 0 \
	./wc_files medium.txt ./remove_block 0 ./wc_files medium.txt ./remove_block 0 ./wc_files small.txt