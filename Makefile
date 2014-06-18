all:
	gcc -g -Wall main.c -lrt -o quid.out

run:
	valgrind ./quid.out

clean:
	rm -rf *.o .rnd quid.out
