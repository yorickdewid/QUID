#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

int delay = GL_DELAY;

void usage(char *pname){
	printf("Usage: %s [options]\n", pname);
	printf("Options:\n");
	printf("  -c <count>               Generation cycles per <count>\n");
	printf("  -d <ms>                  Delay between generation in miliseconds\n");
	printf("  -f <flags>               Set identifier flags\n");
	printf("  -o <file>                output to <file>\n");
	printf("  -s <category>            Set identifier subcategory\n");
	printf("  -q                       Silent, no output shown on screen\n");
	printf("  --rnd-seed <cycles>      Reinitialize rand seed per <cycles>\n");
	printf("  --mem-seed <cycles>      Reinitialize memory seed per <cycles>\n");
	printf("  -h                       Show this help\n");
}

int main(int argc, char *argv[]){
	cuuid_t u;
	int n = 1;
	int c,i;
	char *fname;
	FILE *fp;
	int fout = 0;
	int nout = 0;

	while((c = getopt(argc, argv, "c:d:r:m:f:o:s:qh")) != -1){
		switch(c){
			case 'c':
				n = atoi(optarg);
				break;
			case 'd':
				delay = atoi(optarg);
				break;
			case 'f':
				// flags
				break;
			case 's':
				// subcategory
				break;
			case 'r':
				quid_set_rnd_seed(atoi(optarg));
				break;
			case 'o':
				fname = optarg;
				fout = 1;
				break;
			case 'm':
				quid_set_mem_seed(atoi(optarg));
				break;
			case 'q':
				nout = 1;
				break;
			case 'h':
				usage(argv[0]);
				exit(1);
			default:
				usage(argv[0]);
				exit(1);
		}
	}

	for(i=0; i<n; i++){
		quid_create(&u);
		if((!fout)&&(!nout)){
			quid_print(u);
		}else{
			fp = fopen(fname, "w");
			quid_print_file(fp, u);
			fclose(fp);
		}
		if(delay){
			usleep((delay * 1000));
		}
	}
/*
	printf("Generated QUID # %d\n", n);
	printf("-------------------\n");
	printf("With flags:\n");

	unsigned char data = 0xc5;
	unsigned char cat = 0x9;

	if(data & (1<<0)){
		printf("  PUBLIC\n");
    }else{
        printf("  PRIVATE\n");
    }
    if(data & (1<<2)){
        printf("  MASTER\n");
    }else{
        printf("  SLAVE\n");
    }
    if(data & (1<<4)){
        printf("  SIGNED\n");
    }else{
        printf("  UNSIGNED\n");
    }
    if(data & (1<<5)){
        printf("  TAGGED\n");
    }else{
        printf("  UNTAGGED\n");
    }

	switch(cat){
    case 2:
        printf("COMMON IDENTIFIER\n");
        break;
    case 7:
        printf("INDEX IDENTIFIER\n");
        break;
    case 9:
        printf("SYSTEM IDENTIFIER\n");
        break;
    case 12:
        printf("COMMON INFO\n");
        break;
    case 13:
        printf("COMMON WARNING\n");
        break;
    case 14:
        printf("COMMON ERROR\n");
        break;
    case 22:
        printf("SYSTEM INFO\n");
        break;
    case 23:
        printf("SYSTEM WARNING\n");
        break;
    case 24:
        printf("SYSTEM ERROR\n");
        break;
    default:
        printf("UNKNOWN\n");
        break;
    }

	printf("-------------------\n");
*/
	return 0;
}
