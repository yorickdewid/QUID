#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"

int delay = GL_DELAY;

void usage(char *pname){
	printf("Usage: %s [options]\n", pname);
	printf("Options:\n");
	printf("  -c <count>               Generation cycles per <count>\n");
	printf("  -d <ms>                  Delay between generation in miliseconds\n");
	printf("  -f <flags>               Set identifier flags\n");
	printf("  -o <file>                output to <file>\n");
	printf("  -s <category>            Set identifier subcategory\n");
	printf("  -x                       Output identifier as hexadecimal\n");
	printf("  -i                       Output identifier as number\n");
	printf("  -q                       Silent, no output shown on screen\n");
	printf("  -r <cycles>              Reinitialize rand seed per <cycles>\n");
	printf("  -m <cycles>              Reinitialize memory seed per <cycles>\n");
	printf("  -v                       Output verbose information\n");
	printf("  -V                       Show version\n");
	printf("  -h                       Show this help\n");
}

void print_version(){
	printf("QUID Generator\n");
	printf("Copyright (C) 2014 Quenza, Inc.\n");
	printf("Compiled %s\n", __DATE__);
}

int check_fname(const char *pathname){
	struct stat info;

	if(stat(pathname, &info)!= 0){
		return 0;
	}else if(info.st_mode & S_IFDIR){
		return 1;
	}else{
		return 2;
	}
}

int main(int argc, char *argv[]){
	cuuid_t u;
	int n = 1;
	int c,i;
	char *fname;
	FILE *fp;
	int fout = 0,nout = 0,fmat = 0;

	while((c = getopt(argc, argv, "c:d:r:m:f:o:s:qxiVh")) != -1){
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
			case 'x':
				fmat = 1;
				break;
			case 'i':
				fmat = 2;
				break;
			case 'q':
				nout = 1;
				break;
			case 'V':
				print_version();
				exit(1);
			case 'h':
				usage(argv[0]);
				exit(1);
			default:
				usage(argv[0]);
				exit(1);
		}
	}

	for(i=0; i<n; i++){
		quid_create(&u, IDF_TAGGED | IDF_PUBLIC, CLS_CMON);
		if((!fout)&&(!nout)){
			quid_print(u, fmat);
		}else{
			int rtn = check_fname(fname);
			if(!rtn){
				fp = fopen(fname, "a");
				quid_print_file(fp, u, fmat);
				fclose(fp);
			}else if(rtn==1){
				printf("%s is a directory\n", fname);
			}else if(rtn==2){
				printf("%s already exists\n", fname);
			}
		}
		if(delay){
			usleep((delay * 1000));
		}
	}
/*
	printf("Generated QUID # %d\n", n);
	printf("-------------------\n");
	printf("With flags:\n");

	printf("-------------------\n");
*/
	return 0;
}
