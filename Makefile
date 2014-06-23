all: shared util

shared:
	gcc -g -c -Wall -fpic -DDEBUG quid.c
	gcc -shared -o libquid.so quid.o

util:
	gcc -g -Wall -DDEBUG test/util.c -L$(shell pwd) -lquid -lrt -o test/util.out

run: all
	export LD_LIBRARY_PATH=`pwd`:$(LD_LIBRARY_PATH)
	valgrind ./test/util.out

clean:
	@rm -rf *.o .rnd *.out *.so* *.txt
	@rm -rf test/*.o test/.rnd test/*.out test/*.so* test/*.txt
