compile:
	gcc -g -Wall -DDEBUG main.c -lrt -o quid.out

all: compile

run: compile
	valgrind ./quid.out

clean:
	rm -rf *.o .rnd quid.out
