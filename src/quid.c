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

#ifdef HAVE_CLOCK_GETTIME
#define _DEFAULT_SOURCE
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __WIN32__
#include <unistd.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>
#endif

#include <quid.h>

#include "chacha.h"

#define UIDS_PER_TICK 1024          /* Generate identifiers per tick interval */
#define EPOCH_DIFF 11644473600LL    /* Conversion needed for EPOCH to UTC */
#define RANDFILE ".rnd"             /* File descriptor for random seed */
#define MEM_SEED_CYCLE 65536        /* Generate new memory seed after interval */
#define RND_SEED_CYCLE 4096         /* Generate new random seed after interval */
#define SEEDSZ 16                   /* Seed size */

typedef unsigned long long cuuid_time_t;

/*
 * Temporary node structure
 */
typedef struct {
    char nodeID[6];     /* Allocate 6 nodes */
} cuuid_node_t;

enum {
    QUID_REV1 = 0x7,
    QUID_REV4 = 0x10,
    QUID_REV7 = 0x12,
};

/*
 * Prototypes
 */
static void             format_quid_rev4(cuuid_t *, unsigned short, cuuid_time_t, cuuid_node_t);
static void             format_quid_rev7(cuuid_t *, unsigned short, cuuid_time_t, cuuid_node_t);
static void             get_current_time(cuuid_time_t *);
static unsigned short   true_random(void);

static int max_mem_seed = MEM_SEED_CYCLE;
static int max_rnd_seed = RND_SEED_CYCLE;

/* Set memory seed cycle */
void quid_set_mem_seed(int cnt) {
    max_mem_seed = cnt;
}

/* Set rnd seed cycle */
void quid_set_rnd_seed(int cnt) {
    max_rnd_seed = cnt;
}

/* Library version */
char *quid_libversion(void) {
    return PACKAGE_VERSION;
}

/* Compare */
int quid_cmp(const cuuid_t *s1, const cuuid_t *s2) {
    return s1->time_low == s2->time_low
        && s1->time_mid == s2->time_mid
        && s1->time_hi_and_version == s2->time_hi_and_version
        && s1->clock_seq_hi_and_reserved == s2->clock_seq_hi_and_reserved
        && s1->clock_seq_low == s2->clock_seq_low
        && s1->node[0] == s2->node[0]
        && s1->node[1] == s2->node[1]
        && s1->node[2] == s2->node[2]
        && s1->node[3] == s2->node[3]
        && s1->node[4] == s2->node[4]
        && s1->node[5] == s2->node[5];
}

/* Retrieve system time */
static void get_system_time(cuuid_time_t *uid_time) {
#ifdef __WIN32___
    ULARGE_INTEGER time;

    GetSystemTimeAsFileTime((FILETIME *)&time);
    time.QuadPart += (unsigned __int64) (1000*1000*10)
                    * (unsigned __int64) (60 * 60 * 24)
                    * (unsigned __int64) (17+30+31+365*18+5);
    *uid_time = time.QuadPart;
#else
    struct timeval tv;
    uint64_t result = EPOCH_DIFF;
    gettimeofday(&tv, NULL);
    result += tv.tv_sec;
    result *= 10000000LL;
    result += tv.tv_usec * 10;
    *uid_time = result;
#endif
}

/* Read seed or create if not exist */
static void get_memory_seed(cuuid_node_t *node) {
    static int mem_seed_count = 0;
    static cuuid_node_t saved_node;
    char seed[SEEDSZ];
    FILE *fp;

    if (!mem_seed_count) {
        fp = fopen(RANDFILE, "rb");
        if (fp) {
            if (fread(&saved_node, sizeof(saved_node), 1, fp) < 1)
                abort();
            fclose(fp);
        } else {
            seed[0] |= 0x01;
            memcpy(&saved_node, seed, sizeof(saved_node));

            fp = fopen(RANDFILE, "wb");
            if (fp) {
                if (fwrite(&saved_node, sizeof(saved_node), 1, fp) < 1)
                    abort();
                fclose(fp);
            }
        }
    }

    /* Advance counters */
    mem_seed_count = (mem_seed_count == max_mem_seed) ? 0 : mem_seed_count + 1;

    *node = saved_node;
}

/* QUID REV4 */
int quid_create_rev4(cuuid_t *uid, char flag, char subc) {
    cuuid_time_t    timestamp;
    unsigned short  clockseq;
    cuuid_node_t    node;

    get_current_time(&timestamp);
    get_memory_seed(&node);
    clockseq = true_random();

    format_quid_rev4(uid, clockseq, timestamp, node);

    uid->node[1] |= flag;
    uid->node[2] = subc;

    return QUID_OK;
}

/* QUID REV7 */
int quid_create_rev7(cuuid_t *uid, char flag, char subc) {
    cuuid_time_t    timestamp;
    unsigned short  clockseq;
    cuuid_node_t    node;

    get_current_time(&timestamp);
    get_memory_seed(&node);
    clockseq = true_random();

    format_quid_rev7(uid, clockseq, timestamp, node);

    uid->node[1] |= flag;
    uid->node[2] = subc;

    return QUID_OK;
}

/* Default constructor */
int quid_create(cuuid_t *uid, char flag, char subc) {
// #if defined(LEGACY)
    return quid_create_rev4(uid, flag, subc);
// #else
    // return quid_create_rev7(uid, flag, subc);
// #endif
}

/*
 * Format QUID from the timestamp, clocksequence, and node ID
 * Structure succeeds version 3 (REV1)
 */
void format_quid_rev4(cuuid_t* uid, uint16_t clock_seq, cuuid_time_t timestamp, cuuid_node_t node) {
    uid->time_low = (uint64_t)(timestamp & 0xffffffff);
    uid->time_mid = (uint16_t)((timestamp >> 32) & 0xffff);

    uid->time_hi_and_version = (uint16_t)((timestamp >> 48) & 0xFFF);
    uid->time_hi_and_version ^= 0x80;
    uid->time_hi_and_version |= 0xa000;

    uid->clock_seq_low = (clock_seq & 0xff);
    uid->clock_seq_hi_and_reserved = (clock_seq & 0x3f00) >> 8;
    uid->clock_seq_hi_and_reserved |= 0x80;

    memcpy(&uid->node, &node, sizeof(uid->node));
    uid->node[0] = true_random();
    uid->node[1] = QUID_REV4;
    uid->node[5] = (true_random() & 0xff);
}

/*
 * Format QUID from the timestamp, clocksequence, and node ID
 * Structure succeeds version 3 (REV1)
 */
void format_quid_rev7(cuuid_t* uid, uint16_t clock_seq, cuuid_time_t timestamp, cuuid_node_t node) {
    uid->time_low = (uint64_t)(timestamp & 0xffffffff);
    uid->time_mid = (uint16_t)((timestamp >> 32) & 0xffff);

    uid->time_hi_and_version = (uint16_t)((timestamp >> 48) & 0xFFF);
    uid->time_hi_and_version ^= 0x80;
    uid->time_hi_and_version |= 0xa000;

    uid->clock_seq_low = (clock_seq & 0xff);
    uid->clock_seq_hi_and_reserved = (clock_seq & 0x3f00) >> 8;
    uid->clock_seq_hi_and_reserved |= 0x80;

    memcpy(&uid->node, &node, sizeof(uid->node));
    uid->node[0] = true_random();
    uid->node[1] = QUID_REV7;
    uid->node[3] = 0x17 ^ uid->node[0];
    uid->node[5] = (true_random() & 0xff);

    printf("%x\n", uid->node[1]);
}

/* Get current time including cpu clock */
void get_current_time(cuuid_time_t *timestamp) {
    static int              inited = 0;
    static cuuid_time_t     time_last;
    static uint16_t         ids_this_tick;
    cuuid_time_t            time_now;

    if (!inited) {
        get_system_time(&time_now);
        ids_this_tick = UIDS_PER_TICK;
        inited = 1;
    }

    for (;;) {
        get_system_time(&time_now);

        if (time_last != time_now) {
            ids_this_tick = 0;
            time_last = time_now;
            break;
        }

        if (ids_this_tick < UIDS_PER_TICK) {
            ids_this_tick++;
            break;
        }
    }

    *timestamp = time_now + ids_this_tick;
}

/* Get hardware tick count */
static double get_tick_count(void) {
#ifdef __WIN32___
    return GetTickCount();
#else
    struct timespec now;

    if (clock_gettime(CLOCK_MONOTONIC, &now))
        return 0;

    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
#endif
}

/* Create true random as prescribed by the IEEE */
static uint16_t true_random(void) {
    static int rnd_seed_count = 0;
    cuuid_time_t time_now;

    if (!rnd_seed_count) {
        get_system_time(&time_now);
        time_now = time_now / UIDS_PER_TICK;
        srand((uint32_t)(((time_now >> 32) ^ time_now) & 0xffffffff));
    }

    /* Advance counters */
    rnd_seed_count = (rnd_seed_count == max_rnd_seed) ? 0 : rnd_seed_count + 1;

    return (rand() + get_tick_count());
}

/* Strip special characters from string */
static void strip_special_chars(char *s) {
    char *pr = s, *pw = s;

    while (*pr) {
        *pw = *pr++;
        if ((*pw != '-') &&
            (*pw != '{') &&
            (*pw != '}') &&
            (*pw != ' '))
            pw++;
    }

    *pw = '\0';
}

/* Check if string validates as hex */
static int ishex(char *s) {
    while (*s) {
        if (!isxdigit(*s))
            return 0;
        s++;
    }

    return 1;
}

/* Parse string into QUID */
static void strtoquid(char *str, cuuid_t *u) {
    char octet1[9];
    char octet[5];
    char node[3];

    octet1[8] = '\0';
    octet[4] = '\0';
    node[2] = '\0';

    octet1[0] = str[0];
    octet1[1] = str[1];
    octet1[2] = str[2];
    octet1[3] = str[3];
    octet1[4] = str[4];
    octet1[5] = str[5];
    octet1[6] = str[6];
    octet1[7] = str[7];
    u->time_low = strtol(octet1, NULL, 16);

    octet[0] = str[8];
    octet[1] = str[9];
    octet[2] = str[10];
    octet[3] = str[11];
    u->time_mid = (int)strtol(octet, NULL, 16);

    octet[0] = str[12];
    octet[1] = str[13];
    octet[2] = str[14];
    octet[3] = str[15];
    u->time_hi_and_version = (int)strtol(octet, NULL, 16);

    node[0] = str[16];
    node[1] = str[17];
    u->clock_seq_hi_and_reserved = (char)strtol(node, NULL, 16);

    node[0] = str[18];
    node[1] = str[19];
    u->clock_seq_low = (char)strtol(node, NULL, 16);

    node[0] = str[20];
    node[1] = str[21];
    u->node[0] = (char)strtol(node, NULL, 16);

    node[0] = str[22];
    node[1] = str[23];
    u->node[1] = (char)strtol(node, NULL, 16);

    node[0] = str[24];
    node[1] = str[25];
    u->node[2] = (char)strtol(node, NULL, 16);

    node[0] = str[26];
    node[1] = str[27];
    u->node[3] = (char)strtol(node, NULL, 16);

    node[0] = str[28];
    node[1] = str[29];
    u->node[4] = (char)strtol(node, NULL, 16);

    node[0] = str[30];
    node[1] = str[31];
    u->node[5] = (char)strtol(node, NULL, 16);
}

/* Validate quid as genuine identifier */
int quid_validate(cuuid_t *u) {
    if (u->node[1] != QUID_REV4) //TODO: invalid
        return QUID_ERROR;

    if (!u->node[2])
        return QUID_ERROR;

    return QUID_OK;
}

/* Convert string to identifier */
int quid_parse(char *quid, cuuid_t *uuid) {
    int len;

    /* Remove all special characters */
    strip_special_chars(quid);
    len = strlen(quid);

    /* Fail if invalid length */
    if (len != QUID_LEN)
        return QUID_ERROR;

    /* Fail if not hex */
    if (!ishex(quid))
        return QUID_ERROR;

    strtoquid(quid, uuid);
    if (!quid_validate(uuid))
       return QUID_ERROR;

    return QUID_OK;
}

/* Convert quid to string */
void quid_tostring(const cuuid_t *u, char str[QUID_FULLLEN + 1]) {
    memset(str, '\0', QUID_FULLLEN + 1);
    snprintf(str, QUID_FULLLEN,
        "{%.8x-%.4x-%.4x-%x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x}",
        (unsigned int)u->time_low,
        u->time_mid,
        u->time_hi_and_version,
        u->clock_seq_hi_and_reserved,
        u->clock_seq_low,
        u->node[0],
        u->node[1],
        u->node[2],
        u->node[3],
        u->node[4],
        u->node[5]);
}
