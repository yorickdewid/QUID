all: shared shell

shared:
	gcc -g -c -Wall -fpic -DDEBUG quid.c
	gcc -shared -o libquid.so quid.o

shell:
	gcc -g -Wall -DDEBUG main.c -L$(shell pwd) -lquid -lrt -o main.out

run: all
	export LD_LIBRARY_PATH=`pwd`:$(LD_LIBRARY_PATH)
	valgrind ./main.out

clean:
	@rm -rf *.o .rnd *.out *.so* *.txt
