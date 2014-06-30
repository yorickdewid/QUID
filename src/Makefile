LIBRARY=libquid.so
CFLAGS=-Wall -Wmissing-prototypes -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security

.PHONY: all shared clean

shared:
	gcc $(CFLAGS) -O2 -c -fpic quid.c
	gcc -shared -o $(LIBRARY) quid.o
#	gcc -O2 -fno-strict-aliasing -fwrapv -fexcess-precision=standard -fpic quid.c

shared-debug:
	gcc $(CFLAGS) -g -c -fpic -DDEBUG quid.c
	gcc -shared -o $(LIBRARY) quid.o

util: shared

clean:
	@rm -rf *.o .rnd *.out *.so*
