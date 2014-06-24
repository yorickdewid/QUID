#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"

int delay = GL_DELAY;

void usage(char *pname){
	printf("Usage: %s [options] identifier...\n", pname);
	printf("Options:\n");
	printf("  -c <count>               Generation cycles per <count>\n");
	printf("  --category=<index>       Set identifier with category\n");
	printf("  -d <ms>                  Delay between generation in miliseconds\n");
	printf("  -o <file>                output to <file>\n");
	printf("  --list-categories        Show all categories\n");
	printf("  --set-public             Set public flag\n");
	printf("  --set-safe               Set safety flag\n");
	printf("  --set-master             Set master flag\n");
	printf("  --set-tag                Set tagging flag\n");
	printf("  --set-strict             Set strict data flag\n");
	printf("  -x, --output-hex         Output identifier as hexadecimal\n");
	printf("  -i, --output-number      Output identifier as number\n");
	printf("  -q                       Silent, no output shown on screen\n");
	printf("  --rand-seek=<cycles>     Reinitialize rand seed per <cycles>\n");
	printf("  --memory-seed=<cycles>   Reinitialize memory seed per <cycles>\n");
//	printf("  -V, --verbose            Output verbose information\n");
	printf("  -v, --version            Show version\n");
	printf("  -h, --help               Show this help\n");
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
	int c, i;
	char *fname;
	FILE *fp;
	int fout = 0,nout = 0,fmat = 0;
	int option_index;
	char flg = IDF_NULL;
	char cat = CLS_CMON;

	while(1){
		option_index = 0;
		static struct option long_options[] = {
			{"category",       required_argument, 0, 0},
			{"set-safe",       no_argument,       0, 0},
			{"set-master",     no_argument,       0, 0},
			{"set-public",     no_argument,       0, 0},
			{"set-tag",        no_argument,       0, 0},
			{"set-strict",     no_argument,       0, 0},
			{"list-categories",no_argument,       0, 0},
			{"rand-seed",      required_argument, 0, 0},
			{"memory-seed",    required_argument, 0, 0},
			{"output-hex",     no_argument,       0, 'x'},
			{"output-number",  no_argument,       0, 'i'},
			{"version",        no_argument,       0, 'v'},
			{"help",           no_argument,       0, 'h'},
			{0,                0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "c:d:o:qxivh", long_options, &option_index);
		if(c == -1){
			break;
		}

		switch(c){
			case 0:
				if(!strcmp("rand-seed", long_options[option_index].name)){
					quid_set_rnd_seed(atoi(optarg));
				}else if(!strcmp("memory-seed", long_options[option_index].name)){
					quid_set_mem_seed(atoi(optarg));
				}else if(!strcmp("list-categories", long_options[option_index].name)){
					printf("%d) CLS_CMON\n", CLS_CMON);
					printf("%d) CLS_INFO\n", CLS_INFO);
					printf("%d) CLS_WARN\n", CLS_WARN);
					printf("%d) CLS_ERROR\n", CLS_ERROR);
				}else if(!strcmp("set-safe", long_options[option_index].name)){
					flg |= IDF_IDSAFE;
				}else if(!strcmp("set-public", long_options[option_index].name)){
					flg |= IDF_PUBLIC;
				}else if(!strcmp("set-master", long_options[option_index].name)){
					flg |= IDF_MASTER;
				}else if(!strcmp("set-tag", long_options[option_index].name)){
					flg |= IDF_TAGGED;
				}else if(!strcmp("set-strict", long_options[option_index].name)){
					flg |= IDF_STRICT;
				}else if(!strcmp("category", long_options[option_index].name)){
					cat = atoi(optarg);
				}
				break;
			case 'c':
				n = atoi(optarg);
				break;
			case 'd':
				delay = atoi(optarg);
				break;
			case 'o':
				fname = optarg;
				fout = 1;
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
			case 'v':
				print_version();
				exit(1);
			case 'h':
			case '?':
				usage(argv[0]);
				exit(1);
			default:
				usage(argv[0]);
				exit(1);
		}
	}

	for(i=0; i<n; i++){
		quid_create(&u, flg, cat);
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

	if(optind < argc){
		printf("identifier: ");
		while(optind < argc){
            printf("%s ", argv[optind++]);
		}
		printf("\n");
	}
/*
	printf("Generated QUID # %d\n", n);
	printf("-------------------\n");
	printf("With flags:\n");

	printf("-------------------\n");
*/
	return 0;
}
