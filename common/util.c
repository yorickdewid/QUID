#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "util.h"

int delay = GL_DELAY;
static struct timeval t1,t2;
clock_t ticks = 0;
int n = 1;
char flg = IDF_NULL;
char cat = CLS_CMON;

void input_verbose(cuuid_t);
void generate_verbose(void);
void usage(char *);
void print_version(void);
int check_fname(const char *);
void quid_print(cuuid_t, int);
void quid_print_file(FILE *, cuuid_t, int);

void quid_print(cuuid_t u, int format) {
	quid_print_file(stdout, u, format);
}

/* print QUID to file*/
void quid_print_file(FILE *fp, cuuid_t u, int format) {
	if (format == 1) {
		fprintf(fp, "%x", (unsigned int)u.time_low);
		fprintf(fp, "%x", u.time_mid);
		fprintf(fp, "%x", u.time_hi_and_version);
		fprintf(fp, "%x", u.clock_seq_hi_and_reserved);
		fprintf(fp, "%x", u.clock_seq_low);

		fprintf(fp, "%x", u.node[0]);
		fprintf(fp, "%x", u.node[1]);
		fprintf(fp, "%x", u.node[2]);
		fprintf(fp, "%x", u.node[3]);
		fprintf(fp, "%x", u.node[4]);
		fprintf(fp, "%x", u.node[5]);

		fprintf(fp, "\n");
	} else if(format == 2) {
		fprintf(fp, "%ld", u.time_low);
		fprintf(fp, "%d", u.time_mid);
		fprintf(fp, "%d", u.time_hi_and_version);
		fprintf(fp, "%d", u.clock_seq_hi_and_reserved);
		fprintf(fp, "%d", u.clock_seq_low);

		fprintf(fp, "%d", u.node[0]);
		fprintf(fp, "%d", u.node[1]);
		fprintf(fp, "%d", u.node[2]);
		fprintf(fp, "%d", u.node[3]);
		fprintf(fp, "%d", u.node[4]);
		fprintf(fp, "%d", u.node[5]);

		fprintf(fp, "\n");
	} else {
		fprintf(fp, "{%.8x-", (unsigned int)u.time_low);
		fprintf(fp, "%.4x-", u.time_mid);
		fprintf(fp, "%.4x-", u.time_hi_and_version);
		fprintf(fp, "%x", u.clock_seq_hi_and_reserved);
		fprintf(fp, "%.2x-", u.clock_seq_low);

		fprintf(fp, "%.2x", u.node[0]);
		fprintf(fp, "%.2x", u.node[1]);
		fprintf(fp, "%.2x", u.node[2]);
		fprintf(fp, "%.2x", u.node[3]);
		fprintf(fp, "%.2x", u.node[4]);
		fprintf(fp, "%.2x", u.node[5]);

		fprintf(fp, "}\n");
	}
}


void input_verbose(cuuid_t u) {
	char sflag;

	printf("-----------------------------\n");

	printf("Category index %d\n", u.node[2]);

	sflag = (u.node[1] ^ IDF_NULL);
	printf("Flags");

	if(sflag & FLAG_PUBLIC)
		printf(" PUBLIC");

	if(sflag & FLAG_IDSAFE)
		printf(" IDSAFE");

	if(sflag & FLAG_MASTER)
		printf(" MASTER");

	if(sflag & FLAG_SIGNED)
		printf(" SIGNED");

	if(sflag & FLAG_TAGGED)
		printf(" TAGGED");

	if(sflag & FLAG_STRICT)
		printf(" STRICT");

	printf("\n");

	printf("-----------------------------\n");
}

void generate_verbose(void) {
	double elapsedTime;

	printf("-----------------------------\n");

	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
	elapsedTime = (elapsedTime / 1000);

	printf("Generated %d identifiers\n", n);
	printf("Used %0.2f seconds of CPU time\n", (double)ticks/CLOCKS_PER_SEC);
	printf("Finished in about %0.2f seconds\n", elapsedTime);
	printf("Delayed %d miliseconds\n", delay);

	printf("-----------------------------\n");
}

void usage(char *prog) {
	printf("Usage: %s [options] identifier...\n", prog);
	printf("Options:\n");
	printf("  -c <count>               Generation cycles per <count>\n");
	printf("  --category=<index>       Set identifier with category\n");
	printf("  -d <ms>                  Delay between generation in miliseconds\n");
	printf("  -o <file>                output to <file>\n");
	printf("  --list-categories        Show all categories\n");
	printf("  --set-public             Set public flag\n");
	printf("  --set-safe               Set safety flag\n");
	printf("  --set-master             Set master flag\n");
	printf("  --set-sign               Set signed flag\n");
	printf("  --set-tag                Set tagging flag\n");
	printf("  --set-strict             Set strict data flag\n");
	printf("  -x, --output-hex         Output identifier as hexadecimal\n");
	printf("  -i, --output-number      Output identifier as number\n");
	printf("  -q                       Silent, no output shown on screen\n");
	printf("  --rand-seek=<cycles>     Reinitialize rand seed per <cycles>\n");
	printf("  --memory-seed=<cycles>   Reinitialize memory seed per <cycles>\n");
	printf("  -V, --verbose            Output verbose information\n");
	printf("  -v, --version            Show version\n");
	printf("  -h, --help               Show this help\n");
}

void print_version(void) {
	printf("QUID Generator\n");
	printf("Copyright (C) 2014 Quenza, Inc.\n");
	printf("Compiled %s\n", __DATE__);
}

int check_fname(const char *pathname) {
	struct stat info;

	if(stat(pathname, &info) != 0)
		return 0;
	else if(info.st_mode & S_IFDIR)
		return 1;
	else
		return 2;
}

int main(int argc, char *argv[])
{
	cuuid_t u;
	int c, i, rtn;
	char *fname;
	FILE *fp = NULL;
	int fout = 0, nout = 0, fmat = 0, vbose = 0, gen = 1;
	int option_index;
	static struct option long_options[] = {
		{"category",       required_argument, 0, 0},
		{"set-safe",       no_argument,       0, 0},
		{"set-master",     no_argument,       0, 0},
		{"set-public",     no_argument,       0, 0},
		{"set-sign",       no_argument,       0, 0},
		{"set-tag",        no_argument,       0, 0},
		{"set-strict",     no_argument,       0, 0},
		{"list-categories",no_argument,       0, 0},
		{"rand-seed",      required_argument, 0, 0},
		{"memory-seed",    required_argument, 0, 0},
		{"output-hex",     no_argument,       0, 'x'},
		{"output-number",  no_argument,       0, 'i'},
		{"verbose",        no_argument,       0, 'V'},
		{"version",        no_argument,       0, 'v'},
		{"help",           no_argument,       0, 'h'},
		{0,                0,                 0,  0 }
	};

	while (1) {
		option_index = 0;

		c = getopt_long(argc, argv, "c:d:o:qxivVh", long_options, &option_index);
		if(c == -1)
			break;

		switch(c)
		{
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
					gen = 0;
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
				}else if(!strcmp("set-sign", long_options[option_index].name)){
					flg |= IDF_SIGNED;
				}else if(!strcmp("category", long_options[option_index].name)){
					cat = atoi(optarg);
					if(cat > CLS_ERROR){
						printf("unknown category %d\n", cat);
						printf("see --help for more information\n");
						cat = 0;
					}
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
			case 'V':
				vbose = 1;
				break;
			case 'v':
				print_version();
				gen = 0;
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				gen = 0;
				break;
			default:
				usage(argv[0]);
				gen = 0;
		}
	}

	if(optind < argc){
		cuuid_t uuid;
		while(optind < argc){
			printf("%s\t", argv[optind]);
			if(quid_get_uuid(argv[optind], &uuid)){
				printf("VALID\n");
				if(vbose)
					input_verbose(uuid);
			}else{
				printf("INVALID\n");
			}
			optind++;
		}

		return 0;
	}

	if(gen)
	{
		gettimeofday(&t1, NULL);

		if(fout)
		{
			rtn = check_fname(fname);
			if(!rtn)
				fp = fopen(fname, "a");
			else if(rtn==1){
				printf("%s is a directory\n", fname);

				return 1;
			}else if(rtn==2){
				printf("%s already exists\n", fname);

				return 1;
			}
		}

		for(i=0; i<n; i++)
		{
			quid_create(&u, flg, cat);

			if(!fout){
				if(!nout)
					quid_print(u, fmat);
			}else
				quid_print_file(fp, u, fmat);

			if(delay)
				usleep((delay * 1000));

			ticks = clock();
		}

		if(fp)
			fclose(fp);

		gettimeofday(&t2, NULL);
	}

	if(vbose && gen)
		generate_verbose();

	return 0;
}
