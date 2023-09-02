build: main
	
main.o: main.c
	gcc -Wall -c main.c -o main.o

main: main.o
	gcc main.o -o main

.PHONY: clean

clean:
	rm -f *.o main
