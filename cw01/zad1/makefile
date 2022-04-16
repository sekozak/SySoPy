statyczna:
	make clean
	gcc -c main.c
	gcc -c bibl.c
	ar rcs libbibl.a bibl.o
	gcc main.o libbibl.a -o main

clean:
	rm -rf *.o *.a *.so main

wspoldzielona:
	make clean
	gcc -fPIC -c bibl.c
	gcc -c main.c
	gcc -shared bibl.o -o libbibl.so
	gcc main.o -L. -lbibl -Wl,-rpath=. -o main