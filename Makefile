.PHONY: clean all shared util run

all: shared util

shared:
	gcc -g -c -Wall -fpic -DDEBUG quid.c
	gcc -shared -o libquid.so quid.o
#	gcc -c -g -O2 -Wall -Wmissing-prototypes -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -fpic quid.c

util:
	gcc -g -Wall -DDEBUG test/util.c -L$(shell pwd) -lquid -lrt -o test/util

run: all
	valgrind ./test/util

clean:
	@rm -rf *.o .rnd *.out *.so* *.txt
	@rm -rf test/*.o test/.rnd test/util test/*.so* test/*.txt
