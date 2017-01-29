/*
 * Copyright (c) 2012-2017, Yorick de Wid <ydw at x3 dot quenza dot net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <quid.h>

/* Global variables */
int delay = 0;
static struct timeval t1, t2;
clock_t ticks = 0;
unsigned int i;
char flg = IDF_NULL;
char cat = CLS_CMON;
static int intflag = 0;

/* Prototypes */
void input_verbose(cuuid_t);
void generate_verbose(void);
void usage(void);
void print_version(void);
int check_fname(const char *);
void quid_print(cuuid_t, int);
void quid_print_file(FILE *, cuuid_t, int);
void set_signint(int);
const char *category_name(uint8_t cat);

/* Set flag if program got terminated */
void set_signint(int s) {
    intflag = 1;
}

/* Print QUID on screen */
void quid_print(cuuid_t u, int format) {
    quid_print_file(stdout, u, format);
}

/* Print QUID to file */
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
        fprintf(fp, "{");

        fprintf(fp, "%.8x-", (unsigned int)u.time_low);
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

/* */
void input_verbose(cuuid_t u) {
    char sflag;
    int version = 0;
    char structure[32];

    switch (u.version) {
        case QUID_REV4:
            version = 4;
            strcpy(structure, "memgrep");
            break;
        case QUID_REV7:
            version = 7;
            strcpy(structure, "ChaCha/4");
            break;
    }

    printf("---------------------------------------------\n");
    printf("Health          : %s\n", quid_validate(&u) ? "OK" : "INVALID");
    printf("Timestamp       : %s", asctime(quid_timestamp(&u)));
    printf("Microtime       : %fms\n", quid_microtime(&u)/1000.0);
    printf("Structure       : %s\n", structure);
    printf("Cuuid version   : %d\n", version);
    printf("Tag             : %s\n", quid_tag(&u));
    printf("Category        : %s\n", category_name(u.node[2]));

    /* Remove NULL */
    sflag = (u.node[1] ^ IDF_NULL);
    printf("Flags           :\n");

    if (sflag & FLAG_PUBLIC)
        printf(" * Public\n");

    if (sflag & FLAG_IDSAFE)
        printf(" * Safe\n");

    if (sflag & FLAG_MASTER)
        printf(" * master\n");

    if (sflag & FLAG_SIGNED)
        printf(" * Signed\n");

    if (sflag & FLAG_TAGGED)
        printf(" * Tagged\n");

    if (sflag & FLAG_STRICT)
        printf(" * Strict\n");

    printf("---------------------------------------------\n");
}

/* Show verbose generation information */
void generate_verbose(void) {
    double elapsedTime;

    printf("-----------------------------\n");

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
    elapsedTime = (elapsedTime / 1000);

    printf("Generated %d identifiers\n", i);
    printf("Used %0.2f seconds of CPU time\n", (double)ticks/CLOCKS_PER_SEC);
    printf("Finished in about %0.2f seconds\n", elapsedTime);
    printf("Delayed %d miliseconds\n", delay);

    printf("-----------------------------\n");
}

/* Program usage */
void usage(void) {
    printf("Usage: " PACKAGE_NAME " [OPTIONS] identifier...\n");
    printf("Options:\n");
    printf("  -c <count>               Number of identifiers, 0 for infinite\n");
    printf("  -d <ms>                  Delay between generation in miliseconds\n");
    printf("  --rand-seek=<cycles>     Reinitialize rand seed per <cycles>\n");
    printf("  --memory-seed=<cycles>   Reinitialize memory seed per <cycles>\n");

    printf("\nGeneration:\n");
    printf("  --rev=<version>          Select identifier version\n");
    printf("  --list-categories        Show all categories\n");
    printf("  --tag=<tag>              Set tag\n");
    printf("  --category=<index>       Set identifier with category\n");
    printf("  --set-public             Set public flag\n");
    printf("  --set-safe               Set safety flag\n");
    printf("  --set-master             Set master flag\n");
    printf("  --set-sign               Set signed flag\n");
    printf("  --set-tag                Set tagging flag\n");
    printf("  --set-strict             Set strict data flag\n");

    printf("\nOutput:\n");
    printf("  -o <file>                Output to <file>\n");
    printf("  -x, --output-hex         Output identifier as hexadecimal\n");
    printf("  -i, --output-number      Output identifier as number\n");
    printf("  -q                       Silent, no output shown on screen\n");

    printf("\n");
    printf("  -V, --verbose            Output verbose information\n");
    printf("  -v, --version            Show version\n");
    printf("  -h, --help               Show this help\n\n");
    printf("Report bugs to <" PACKAGE_BUGREPORT ">\n");
}

/* Print program version */
void print_version(void) {
    printf("QUID Generator %s\n", PACKAGE_VERSION);
    printf("Copyright (C) 2017 Quenza, Inc.\n");
    printf("Report bugs to <" PACKAGE_BUGREPORT ">\n");
}

/* Check filesystem for output file */
int check_fname(const char *pathname) {
    struct stat info;

    if (stat(pathname, &info) != 0)
        return 0;
    else if(info.st_mode & S_IFDIR)
        return 1;
    else
        return 2;
}

const char *category_name(uint8_t cat) {
    switch (cat) {
        case CLS_CMON:
            return "Common";
        case CLS_INFO:
            return "Info";
        case CLS_WARN:
            return "Warning";
        case CLS_ERROR:
            return "Error";
        default:
            return "Unknown";
    }
}

/* Program main */
int main(int argc, char *argv[]) {
    cuuid_t cuuid;
    int c, rtn;
    unsigned int n = 1;
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
        {"rev",            required_argument, 0, 0},
        {"rand-seed",      required_argument, 0, 0},
        {"memory-seed",    required_argument, 0, 0},
        {"output-hex",     no_argument,       0, 'x'},
        {"output-number",  no_argument,       0, 'i'},
        {"verbose",        no_argument,       0, 'V'},
        {"version",        no_argument,       0, 'v'},
        {"help",           no_argument,       0, 'h'},
        {0,                0,                 0,  0 }
    };

    signal(SIGINT, set_signint);

    while (1) {
        option_index = 0;

        c = getopt_long(argc, argv, "c:d:o:qxivVh", long_options, &option_index);
        if(c == -1)
            break;

        switch (c) {
            case 0:
                if (!strcmp("rand-seed", long_options[option_index].name))
                    quid_set_rnd_seed(atoi(optarg));
                else if (!strcmp("memory-seed", long_options[option_index].name))
                    quid_set_mem_seed(atoi(optarg));
                else if (!strcmp("rev", long_options[option_index].name))
                    switch (atoi(optarg)) {
                        case 4:
                            cuuid.version = QUID_REV4;
                            break;
                        case 7:
                        default:
                            cuuid.version = QUID_REV7;
                            break;
                    }
                else if (!strcmp("list-categories", long_options[option_index].name)){
                    printf("%d) %s\n", CLS_CMON, category_name(CLS_CMON));
                    printf("%d) %s\n", CLS_INFO, category_name(CLS_INFO));
                    printf("%d) %s\n", CLS_WARN, category_name(CLS_WARN));
                    printf("%d) %s\n", CLS_ERROR, category_name(CLS_ERROR));
                    gen = 0;
                }else if (!strcmp("set-safe", long_options[option_index].name))
                    flg |= IDF_IDSAFE;
                else if (!strcmp("set-public", long_options[option_index].name))
                    flg |= IDF_PUBLIC;
                else if (!strcmp("set-master", long_options[option_index].name))
                    flg |= IDF_MASTER;
                else if (!strcmp("set-tag", long_options[option_index].name))
                    flg |= IDF_TAGGED;
                else if (!strcmp("set-strict", long_options[option_index].name))
                    flg |= IDF_STRICT;
                else if (!strcmp("set-sign", long_options[option_index].name))
                    flg |= IDF_SIGNED;
                else if (!strcmp("category", long_options[option_index].name)){
                    cat = atoi(optarg);
                    if (cat > CLS_ERROR) {
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
                return 0;
            case 'h':
            case '?':
            default:
                usage();
                return 0;
        }
    }

    /* Identifier as positional argument */
    if (optind < argc) {
        cuuid_t uuid;
        while (optind < argc) {
            printf("%s\t", argv[optind]);
            if (quid_parse(argv[optind], &uuid)) {
                printf("VALID\n");
                if (vbose)
                    input_verbose(uuid);
            } else
                printf("INVALID\n");
            optind++;
        }

        return 0;
    }

    /* Output new identifiers */
    if (gen) {
        gettimeofday(&t1, NULL);

        /* File output */
        if (fout) {
            rtn = check_fname(fname);
            if (!rtn) {
                fp = fopen(fname, "a");
            } else if (rtn == 1) {
                printf("%s is a directory\n", fname);
                return 1;
            } else if (rtn == 2){
                printf("%s already exists\n", fname);
                return 1;
            }
        }

        for (i=0; i<n; ++i) {
            quid_create(&cuuid, flg, cat, "KAS");

            if (intflag)
                break;

            if (!fout){
                if (!nout)
                    quid_print(cuuid, fmat);
            } else
                quid_print_file(fp, cuuid, fmat);

            if (delay)
                usleep(delay * 1000);

            ticks = clock();
        }

        if (fp)
            fclose(fp);

        gettimeofday(&t2, NULL);
    }

    if (vbose && gen) 
        generate_verbose();

    return 0;
}
