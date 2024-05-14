compile:
	clang -Wall -pedantic-errors server.c -o server.out
run:
	make
	./server.c
clean:
	rm *.out
