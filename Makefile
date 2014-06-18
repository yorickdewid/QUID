all:
	gcc main.c -lrt -o guid.out

run:
	valgrind ./guid.out

clean:
	rm -rf *.o guid.out
