/*
 * Copyright (c) 2012-2020, Yorick de Wid <yorick17 at outlook dot com>
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
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
# include <winsock2.h>
# include "win32_getopt.h"
#else
# include <unistd.h>
# include <getopt.h>
# include <sys/time.h>
#endif

#include <quid.h>
#include <config.h>

#ifdef WIN32
# if !defined S_ISDIR
#  define S_ISDIR(m) (((m) & _S_IFDIR) == _S_IFDIR)
# endif
#endif

/* Global variables */
int delay = 0;
static struct timeval t1, t2;
clock_t ticks = 0;
unsigned long long int i;
char flg = IDF_NULL;
char cat = CLS_CMON;
static int intflag = 0;

enum {
    PRINT_FORMAT_HEX = 1,
    PRINT_FORMAT_DEC = 2,
    PRINT_FORMAT_HEX_BACKET = 0,
};

static void quid_print_file_hex(FILE *fp, cuuid_t u) {
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
}

static void quid_print_file_dec(FILE *fp, cuuid_t u) {
#if defined(WIN32) || defined(__APPLE__)
    fprintf(fp, "%lld", u.time_low);
#else
    fprintf(fp, "%ld", u.time_low);
#endif
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
}

static void quid_print_file_hex_bracket(FILE *fp, cuuid_t u) {
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

/* Print QUID to file */
static void quid_print_file(FILE *fp, cuuid_t u, int format) {
    switch (format) {
        case PRINT_FORMAT_HEX:
            quid_print_file_hex(fp, u);
            break;
        case PRINT_FORMAT_DEC:
            quid_print_file_dec(fp, u);
            break;
        case PRINT_FORMAT_HEX_BACKET:
        default:
            quid_print_file_hex_bracket(fp, u);
            break;
    }
}

/* Print QUID on screen */
static void quid_print(cuuid_t u, int format) {
    quid_print_file(stdout, u, format);
}

static const char *category_name(uint8_t _cat) {
    switch (_cat) {
        case CLS_CMON:
            return "Common";
        case CLS_INFO:
            return "Info";
        case CLS_WARN:
            return "Warning";
        case CLS_ERROR:
            return "Error";
        default:
            break;
    }

    return "Unknown";
}

/* Report versbose identifier info */
static void input_verbose(cuuid_t u) {
    char sflag;
    int version = 0;
    char structure[32];
    char rev[32];

    switch (u.version) {
        case QUID_REV4:
            version = 4;
            strcpy(structure, "memgrep");
            strcpy(rev, "REV2012");
            break;
        case QUID_REV7:
            version = 7;
            strcpy(structure, "ChaCha/4");
            strcpy(rev, "REV2017");
            break;
    }

    printf("---------------------------------------------\n");
    printf("Health          : %s\n", quid_validate(&u) ? "OK" : "INVALID");
    printf("Timestamp (UTC) : %s", asctime(quid_timestamp(&u)));
    printf("Microtime       : %fms\n", quid_microtime(&u)/1000.0);
    printf("Structure       : %s\n", structure);
    printf("Cuuid version   : %d (%s)\n", version, rev);
    printf("Tag             : %s\n", quid_tag(&u));
    printf("Category        : %s\n", category_name(quid_category(&u)));

    /* Remove NULL */
    sflag = (quid_flag(&u) ^ IDF_NULL);
    printf("Flags           :\n");

    if (sflag & FLAG_PUBLIC) {
        printf(" * Public\n");
    }

    if (sflag & FLAG_IDSAFE) {
        printf(" * Safe\n");
    }

    if (sflag & FLAG_MASTER) {
        printf(" * master\n");
    }

    if (sflag & FLAG_SIGNED) {
        printf(" * Signed\n");
    }

    if (sflag & FLAG_TAGGED) {
        printf(" * Tagged\n");
    }

    if (sflag & FLAG_STRICT) {
        printf(" * Strict\n");
    }

    printf("---------------------------------------------\n");
}

/* Show verbose generation information */
static void print_verbose(void) {
    double elapsedTime;

    printf("-----------------------------\n");

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
    elapsedTime = (elapsedTime / 1000);

    printf("Generated %llu identifiers\n", i);
    printf("Used %0.2f seconds of CPU time\n", (double)ticks/CLOCKS_PER_SEC);
    printf("Finished in about %0.2f seconds\n", elapsedTime);
    printf("Delayed %d miliseconds\n", delay);

    printf("-----------------------------\n");
}

/* Program usage */
static void usage(void) {
    printf("Usage: " PROJECT_NAME " [OPTIONS] identifier...\n");
    printf("QUID generation and validation tool.\n");
    
    printf("\nOptions:\n");
    printf("  -c <count>               Number of identifiers\n");
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
    printf("Report bugs to <" PROJECT_BUGREPORT ">\n");
}

/* Print program version */
static void print_version(void) {
    printf("QUID Generator %s\n", PROJECT_VERSION);
    printf("Copyright (C) " PROJECT_COMPILE_YEAR " " PROJECT_AUTHOR "\n");
    printf("Report bugs to <" PROJECT_BUGREPORT ">\n");
}

/* Check filesystem for output file */
static int check_fname(const char *pathname) {
    struct stat info;

    if (stat(pathname, &info) != 0) {
        return 0;
    } else if (S_ISDIR(info.st_mode)) {
        return 1;
    }

    return 2;
}

static void qusleep(int64_t usec) {
#ifdef WIN32
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#else
    struct timespec tm;
    tm.tv_sec = 0;
    tm.tv_nsec = usec * 1000;

    nanosleep(&tm, NULL);
#endif
}

#ifdef WIN32
int gettimeofday(struct timeval *tp, char *tzp) {
    ((void)tzp);

    // NOTE: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t)11644473600ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}
#endif

/* Set flag if program got terminated */
static void set_signint(int s) {
    ((void)s);
    intflag = 1;
}

/* Program main */
int main(int argc, char *argv[]) {
    cuuid_t cuuid;
    int c, rtn;
    unsigned long long n = 1;
    char *fname = NULL;
    FILE *fp = NULL;
    int fout = 0, nout = 0, fmat = PRINT_FORMAT_HEX_BACKET, vbose = 0, gen = 1;
    int option_index;
    char tag[3] = {0x0, 0x0, 0x0};

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
        {"tag",            required_argument, 0, 0},
        {"rand-seed",      required_argument, 0, 0},
        {"memory-seed",    required_argument, 0, 0},
        {"output-hex",     no_argument,       0, 'x'},
        {"output-number",  no_argument,       0, 'i'},
        {"verbose",        no_argument,       0, 'V'},
        {"version",        no_argument,       0, 'v'},
        {"help",           no_argument,       0, 'h'},
        {0,                0,                 0,  0 }
    };

    /* Register interrupt handler */
    signal(SIGINT, set_signint);

    while (1) {
        option_index = 0;

        c = getopt_long(argc, argv, "c:d:o:qxivVh", long_options, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
            case 0:
                if (!strcmp("rand-seed", long_options[option_index].name)) {
                    quid_set_rnd_seed(atoi(optarg));
                } else if (!strcmp("memory-seed", long_options[option_index].name)) {
                    quid_set_mem_seed(atoi(optarg));
                } else if (!strcmp("rev", long_options[option_index].name)) {
                    switch (atoi(optarg)) {
                        case 4:
                            cuuid.version = QUID_REV4;
                            break;
                        case 7:
                        default:
                            cuuid.version = QUID_REV7;
                            break;
                    }
                } else if (!strcmp("tag", long_options[option_index].name)) {
                    if (strlen(optarg) != 3) {
                        printf("tag must be exactly 3 characters, %zu given\n", strlen(optarg));
                        printf("see --help for more information\n");
                        return 1;
                    }
                    tag[0] = optarg[0];
                    tag[1] = optarg[1];
                    tag[2] = optarg[2];
                } else if (!strcmp("list-categories", long_options[option_index].name)) {
                    printf("%d) %s\n", CLS_CMON, category_name(CLS_CMON));
                    printf("%d) %s\n", CLS_INFO, category_name(CLS_INFO));
                    printf("%d) %s\n", CLS_WARN, category_name(CLS_WARN));
                    printf("%d) %s\n", CLS_ERROR, category_name(CLS_ERROR));
                    return 0;
                } else if (!strcmp("set-safe", long_options[option_index].name)) {
                    flg |= IDF_IDSAFE;
                } else if (!strcmp("set-public", long_options[option_index].name)) {
                    flg |= IDF_PUBLIC;
                } else if (!strcmp("set-master", long_options[option_index].name)) {
                    flg |= IDF_MASTER;
                } else if (!strcmp("set-tag", long_options[option_index].name)) {
                    flg |= IDF_TAGGED;
                } else if (!strcmp("set-strict", long_options[option_index].name)) {
                    flg |= IDF_STRICT;
                } else if (!strcmp("set-sign", long_options[option_index].name)) {
                    flg |= IDF_SIGNED;
                } else if (!strcmp("category", long_options[option_index].name)) {
                    cat = (char)atoi(optarg);
                    switch (cat) {
                        case CLS_CMON:
                        case CLS_INFO:
                        case CLS_WARN:
                        case CLS_ERROR:
                            break;
                        default:
                            printf("unknown category %d\n", cat);
                            printf("see --help for more information\n");
                            return 1;
                    }
                }
                break;
            case 'c':
                n = atoll(optarg);
                break;
            case 'd':
                delay = atoi(optarg);
                break;
            case 'o':
                fname = optarg;
                fout = 1;
                break;
            case 'x':
                fmat = PRINT_FORMAT_HEX;
                break;
            case 'i':
                fmat = PRINT_FORMAT_DEC;
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
                if (vbose) {
                    input_verbose(uuid);
                }
            } else {
                printf("INVALID\n");
            }
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
                if (fp == NULL) {
                    printf("%s is not writable\n", fname);
                    return 1;
                }
            } else if (rtn == 1) {
                printf("%s is a directory\n", fname);
                return 1;
            } else if (rtn == 2){
                printf("%s already exists\n", fname);
                return 1;
            }
        }

        for (i=0; i<n; ++i) {
            assert(cat != 0);
            quid_create(&cuuid, flg, cat, tag);

            if (intflag) {
                break;
            }

            if (!fout){
                if (!nout) {
                    quid_print(cuuid, fmat);
                }
            } else {
                quid_print_file(fp, cuuid, fmat);
            }

            if (delay) {
                qusleep(delay * 1000);
            }

            ticks = clock();
        }

        if (fp) {
            fclose(fp);
        }

        gettimeofday(&t2, NULL);
    }

    /* Show counters */
    if (vbose && gen) {
        print_verbose();
    }

    return 0;
}
