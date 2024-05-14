compile:
	clang -Wall -pedantic-errors server.c -lwiringPi -o server.out
run:
	make
	./server.out
clean:
	rm *.out
