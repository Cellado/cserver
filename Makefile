compile:
	clang -Wall -pedantic-errors server.c -lpigpio -lrt -lpthread -o server.out
run:
	make
	sudo ./server.out
clean:
	rm *.out
