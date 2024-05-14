compile:
	clang -Wall -pedantic-errors server.c -lpigpio -lrt -lpthread -o server.out
run:
	make
	./server.c
clean:
	rm *.out
