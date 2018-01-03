/*
 * Copyright (c) 2012-2018, Yorick de Wid <yorick17 at outlook dot com>
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

/*
 * CHANGELOG:
 *
 * 2012-08: Version 1.0
 * 2017-01: Version 1.3
 *          - QUID version 7
 *          - Fix string functions
 *          - Testcases
 * 2017-01: Version 1.4
 *          - User defined tags
 * 2017-12: Version 1.5
 *          - Update Licenses
 *          - Add CMake support
 *          - Windows support
 *
 * TODO:
 * - Last digit in timestamp
 * - Move random cipher counter
 * - Request version from quid
 * - #if guards around obsolete functions
 * - REV8 2018 version
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#ifdef _WIN32
# include <winsock2.h>
#else
# include <unistd.h>
# include <sys/time.h>
# include <ctype.h>
#endif

#include <quid.h>

#include "chacha.h"

#define UIDS_PER_TICK   1024             /* Generate identifiers per tick interval */
#define RANDFILE        ".rnd"           /* File descriptor for random seed */
#define MEM_SEED_CYCLE  65536            /* Generate new memory seed after interval */
#define RND_SEED_CYCLE  4096             /* Generate new random seed after interval */
#define SEEDSZ          16               /* Seed size */
#define QUIDMAGIC       0x80             /* QUID Timestamp magic */

#define VERSION_REV4    0xa000
#define VERSION_REV7    0xb000

typedef unsigned long long cuuid_time_t;

#ifdef _WIN32
# define PRINT_QUID_FORMAT "{%.8llx-%.4x-%.4x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x}"
# define QFOPEN(f,n,m) assert(fopen_s(&f, n, m) == 0);
# define q_gettimeofday(t,z) win32_gettimeofday(t,z)
#else
# define PRINT_QUID_FORMAT "{%.8lx-%.4x-%.4x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x}"
# define QFOPEN(f,n,m) f = fopen(n, m);
# define q_gettimeofday(t,z) gettimeofday(t,z)
# define HAS_GETTIMEOFDAY 1
#endif

#define UNUSED(u) ((void)u)

#define SIZE_CHECK() \
    assert(sizeof(uint64_t) == 8); \
    assert(sizeof(long long) == 8);

/**
 * Temporary node structure
 */
typedef struct {
    uint8_t node[6];     /* Allocate 6 nodes */
} cuuid_node_t;

/**
 * Prototypes
 */
static void             format_quid_rev4(cuuid_t *, uint16_t, cuuid_time_t, cuuid_node_t);
static void             format_quid_rev7(cuuid_t *, uint16_t, cuuid_time_t);
static void             encrypt_node(uint64_t, uint8_t, uint8_t, cuuid_node_t *);
static void             get_current_time(cuuid_time_t *);
static unsigned short   true_random(void);

static int max_mem_seed = MEM_SEED_CYCLE;
static int max_rnd_seed = RND_SEED_CYCLE;

static const uint8_t padding[3] = {0x12, 0x82, 0x7b};

/* Set memory seed cycle (OBSOLETE) */
QUID_LIB_API void quid_set_mem_seed(int cnt) {
    max_mem_seed = cnt;
}

/* Set rnd seed cycle */
QUID_LIB_API void quid_set_rnd_seed(int cnt) {
    max_rnd_seed = cnt;
}

/* Library version */
QUID_LIB_API const char *quid_libversion(void) {
    return PACKAGE_VERSION;
}

/**
* Check whether memory is a vector of same values.
*
* @param  memory  Memory region to inspect
* @param  val     Value to detect in all memory locations
* @param  size    Size of memory block
* @return         1 if all memory elements contain the same value, otherwise 0
*/
static int memvcmp(void *memory, unsigned char val, unsigned int size)
{
    uint8_t *mm = (uint8_t *)memory;
    return (*mm == val) && memcmp(mm, mm + 1, size - 1) == 0;
}

/**
* Compare two quid structures and return match result.
*
* @param   s1  First quid to be compared
* @param   s2  Second quid to be compared with first
* @return      0 if the two identifiers did not match, otherwise 1
*/
QUID_LIB_API int quid_cmp(const cuuid_t *s1, const cuuid_t *s2) {
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

#ifndef HAS_GETTIMEOFDAY
int win32_gettimeofday(struct timeval *tp, char *tzp) {
    SYSTEMTIME system_time;
    FILETIME file_time;
    uint64_t time;
    UNUSED(tzp);

    /**
     * Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
     * This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
     * until 00:00:00 January 1, 1970
     */
    static const uint64_t EPOCH_DIFF = ((uint64_t)116444736000000000);

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH_DIFF) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);

    assert(tp->tv_sec > 1000000000);
    return 0;
}
#endif

/**
 * Get system time in Coordinated Universal Time (UTC).
 * Depending on the system some form of the gettimeofday
 * function is called.
 *
 * @return  cuuid_time_t containing the 64 bit timestamp
 */
static void get_system_time(cuuid_time_t *cuuid_time) {
    struct timeval tv;
    if (q_gettimeofday(&tv, NULL) != 0) {
        assert(0);
    }
    
    /* Squeeze sec and usec into single integer */
    uint64_t result = tv.tv_sec;
    result *= 10000000LL;
    result += tv.tv_usec * 10;
    *cuuid_time = result;

    assert(result > 10000000000000000);
    assert(cuuid_time);
}

/**
 * Retrieve timestamp from QUID
 * 
 * @param  cuuid   Quid input structure
 * @return         struct timeval
 */
static void quid_timeval(cuuid_t *cuuid, struct timeval *tv) {
    cuuid_time_t cuuid_time;
    uint16_t versubtr = 0;
    long int usec;
    time_t sec;
    assert(cuuid);

    /* Determine version substraction */
    switch (cuuid->version) {
        case QUID_REV4:
            versubtr = VERSION_REV4;
            break;
        case QUID_REV7:
            versubtr = VERSION_REV7;
            break;
    }

    /* Reconstruct timestamp */
    cuuid_time = (uint64_t)cuuid->time_low | (uint64_t)cuuid->time_mid << 32 | 
    (uint64_t)((cuuid->time_hi_and_version ^ QUIDMAGIC) - versubtr) << 48;

    /* Timestamp to timeval */
    usec = (cuuid_time/10) % 1000000LL;
    sec = (((cuuid_time/10) - usec)/1000000LL);// - EPOCH_DIFF;

    tv->tv_sec = (long)sec;
    tv->tv_usec = usec;
}

/**
 * Retrieve timestamp as timeinfo structure.
 * The timeinfo structure can be converted into
 * a string or serve as input to other datetime
 * functions.
 * 
 * @param  cuuid   Quid input structure
 * @return         struct tm on success or NULL on faillure
 */
QUID_LIB_API struct tm *quid_timestamp(cuuid_t *cuuid) {
    struct timeval tv;
    assert(cuuid);

    /* Fetch time from quid */
    quid_timeval(cuuid, &tv);
    const time_t timet = tv.tv_sec;

    /* Localtime */
#ifdef _WIN32
    static struct tm timeinfo;
    if (gmtime_s(&timeinfo, &timet) != 0) {
        return NULL;
    };
    return &timeinfo;
#else
    return gmtime(&timet);
#endif
}

/* Retrieve microtime */
QUID_LIB_API long quid_microtime(cuuid_t *cuuid) {
    struct timeval tv;
    assert(cuuid);

    /* Fetch time from quid */
    quid_timeval(cuuid, &tv);

    /* Microseconds */
    return tv.tv_usec;
}

/* Retrieve user tag */
QUID_LIB_API const char *quid_tag(cuuid_t *cuuid) {
    cuuid_node_t node;
    static char tag[3];
    assert(cuuid);

    /* Skip older formats */
    if (cuuid->version != QUID_REV7) {
        return "Not implemented";
    }

    assert(memcpy(&node, &cuuid->node, sizeof(cuuid_node_t)));
    encrypt_node(cuuid->time_low, cuuid->clock_seq_hi_and_reserved, cuuid->clock_seq_low, &node);

    /* Must match version */
    if (node.node[0] != QUID_REV7) {
        return "Invalid";
    }

    /* Check for padding */
    if (node.node[3] == padding[0] &&
        node.node[4] == padding[1] &&
        node.node[5] == padding[2]) {
        return "None";
    }

    tag[0] = node.node[3];
    tag[1] = node.node[4];
    tag[2] = node.node[5];

    return tag;
}

/* Retrieve category */
QUID_LIB_API uint8_t quid_category(cuuid_t *cuuid) {
    cuuid_node_t node;
    assert(cuuid);

    /* Determine category per version */
    switch (cuuid->version) {
        case QUID_REV4:
            return cuuid->node[2];
        case QUID_REV7: {
            assert(memcpy(&node, &cuuid->node, sizeof(cuuid_node_t)));
            encrypt_node(cuuid->time_low, cuuid->clock_seq_hi_and_reserved, cuuid->clock_seq_low, &node);
            return node.node[2];
        }
    }

    /* Invalid */
    return QUID_ERROR;
}

/* Retrieve flag if any */
QUID_LIB_API uint8_t quid_flag(cuuid_t *cuuid) {
    cuuid_node_t node;

    /* Determine category per version */
    switch (cuuid->version) {
        case QUID_REV4:
            return cuuid->node[1];
        case QUID_REV7: {
            assert(memcpy(&node, &cuuid->node, sizeof(cuuid_node_t)));
            encrypt_node(cuuid->time_low, cuuid->clock_seq_hi_and_reserved, cuuid->clock_seq_low, &node);
            return node.node[1];
        }
    }

    /* Invalid */
    return QUID_ERROR;
}

/**
 * Read seed or create if not exist.
 *
 * OBSOLETE:
 *    Compilers and platforms may zero the stack
 *    and/or heap memory beforehand causing low
 *    entropy QUID nodes. The function is still
 *    part of the source to support backwards
 *    compatible quid structures.
 */
static void get_memory_seed(cuuid_node_t *node) {
    static int mem_seed_count = 0;
    static cuuid_node_t saved_node;
    uint8_t seed[SEEDSZ];
    FILE *fp = NULL;

    if (!mem_seed_count) {
        QFOPEN(fp, RANDFILE, "rb");
        if (fp) {
            assert(fread(&saved_node, sizeof(saved_node), 1, fp) > 0);
            assert(fclose(fp) == 0);
        } else {
            seed[0] |= 0x01;
            assert(memcpy(&saved_node, seed, sizeof(saved_node)));

            QFOPEN(fp, RANDFILE, "wb");
            if (fp) {
                assert(fwrite(&saved_node, sizeof(saved_node), 1, fp) > 0);
                assert(fclose(fp) == 0);
            }
        }
    }

    /* Advance counter */
    mem_seed_count = (mem_seed_count == max_mem_seed) ? 0 : mem_seed_count + 1;

    *node = saved_node;
    assert(node);
}

/**
 * Run the nods agains ChaCha in order to XOR encrypt or decrypt
 * The key and IV are stretched to match the stream input. The derivations
 * are by no means secure and are only applied to increase diffusion. The stream
 * cipher may use low rounds as the primary goal is entropy, not privacy.
 *
 * @param  prekey    The key to encrypt datablocks
 * @param  preiv1    Initialization vector higher bits
 * @param  preiv2    Initialization vector lower bits
 * @param  node      Node block to encrypt, the parameter is permuted in place
 */
static void encrypt_node(uint64_t prekey, uint8_t preiv1, uint8_t preiv2, cuuid_node_t *node) {
    chacha_ctx ctx;
    uint8_t key[16];
    uint8_t iv[8];

    assert(prekey);
    //assert(preiv1);
    //assert(preiv2);
    assert(node);

    /* Weak key stretching */
    key[0] = 0x0;
    key[1] = (uint8_t)prekey | (uint8_t)(prekey >> 16);
    key[2] = (uint8_t)prekey ^ (uint8_t)(prekey >> 8);
    key[3] = (uint8_t)prekey;

    key[4] = 0x1;
    key[5] = (uint8_t)(prekey >> 8) | (uint8_t)(prekey >> 24);
    key[6] = (uint8_t)(prekey >> 8) ^ (uint8_t)(prekey >> 16);
    key[7] = (uint8_t)(prekey >> 8);

    key[8] = 0x2;
    key[9] = (uint8_t)(prekey >> 16) | (uint8_t)prekey;
    key[10] = (uint8_t)(prekey >> 16) ^ (uint8_t)(prekey >> 24);
    key[11] = (uint8_t)(prekey >> 16);

    key[12] = 0x3;
    key[13] = (uint8_t)(prekey >> 24) | (uint8_t)(prekey >> 8);
    key[14] = (uint8_t)(prekey >> 24) ^ (uint8_t)prekey;
    key[15] = (uint8_t)(prekey >> 24);

    /* IV derivation */
    iv[0] = 0x0;
    iv[1] = 0x1;
    iv[2] = preiv1 ^ preiv2;
    iv[3] = preiv1;

    iv[4] = 0x4;
    iv[5] = 0x5;
    iv[6] = preiv2 & preiv1;
    iv[7] = preiv2;

    /* Prepare stream */
    chacha_init_ctx(&ctx, 4);
    chacha_init(&ctx, key, 128, iv, 0);

    /* Encrypt the node with the stretched key */
    chacha_xor(&ctx, (uint8_t *)node, sizeof(cuuid_node_t));
    assert(node);
}

/* QUID format REV4 */
QUID_LIB_API int quid_create_rev4(cuuid_t *uid, uint8_t flag, uint8_t subc) {
    cuuid_time_t    timestamp;
    unsigned short  clockseq;
    cuuid_node_t    node;

    assert(memvcmp(uid, '\0', sizeof(cuuid_t)));

    uid->version = QUID_REV4;
    get_current_time(&timestamp);
    get_memory_seed(&node);
    clockseq = true_random();

    /* Format QUID */
    format_quid_rev4(uid, clockseq, timestamp, node);

    /* Set flags and subclasses */
    uid->node[1] = flag;
    uid->node[2] = subc;

    return QUID_OK;
}

/* QUID format REV7 */
QUID_LIB_API int quid_create_rev7(cuuid_t *uid, uint8_t flag, uint8_t subc, char tag[3]) {
    cuuid_time_t    timestamp;
    unsigned short  clockseq;
    cuuid_node_t    node;
    
    assert(memvcmp(uid, '\0', sizeof(cuuid_t)));

    uid->version = QUID_REV7;
    get_current_time(&timestamp);
    clockseq = true_random();

    /* Format QUID */
    format_quid_rev7(uid, clockseq, timestamp);

    /* Prepare nodes */
    node.node[0] = QUID_REV7;
    node.node[1] = flag;
    node.node[2] = subc;
    node.node[3] = padding[0];
    node.node[4] = padding[1];
    node.node[5] = padding[2];

    /* Set tag if passed */
    if (tag && tag[0] != 0 && tag[1] != 0 && tag[2] != 0) {
        node.node[3] = tag[0];
        node.node[4] = tag[1];
        node.node[5] = tag[2];
    }

    /* Encrypt nodes */
    encrypt_node(uid->time_low, uid->clock_seq_hi_and_reserved, uid->clock_seq_low, &node);
    assert(memcpy(&uid->node, &node, sizeof(uid->node)));

    return QUID_OK;
}

/**
 * Default constructor for new quid structures.
 *
 * @param  cuuid The quid output structure, the caller must provide the memory block
 * @param  flag  Indicator flag to encode boolean flags inside the quid
 * @param  subc  Subclass to encode in the quid, parameter may not be zero
 * @param  tag   Optional tag to include in the quid structure
 * @return       QUID_ERROR on faillure and QUID_OK on success
 */
QUID_LIB_API int quid_create(cuuid_t *cuuid, uint8_t flag, uint8_t subc, char tag[3]) {

    /* Static size assert */
    SIZE_CHECK();

    assert(memset(cuuid, '\0', sizeof(cuuid_t)));
    if (cuuid->version == QUID_REV4) {//TODO
        return quid_create_rev4(cuuid, flag, subc);
    }

    /* Default to latest */
    return quid_create_rev7(cuuid, flag, subc, tag);
}

/*
 * Format QUID from the timestamp, clocksequence, and node ID
 * Structure succeeds version 3 (REV1)
 */
static void format_quid_rev4(cuuid_t* uid, uint16_t clock_seq, cuuid_time_t timestamp, cuuid_node_t node) {
    uid->time_low = (uint64_t)(timestamp & 0xffffffff);
    uid->time_mid = (uint16_t)((timestamp >> 32) & 0xffff);

    uid->time_hi_and_version = (uint16_t)((timestamp >> 48) & 0xFFF);
    uid->time_hi_and_version ^= QUIDMAGIC;
    uid->time_hi_and_version |= VERSION_REV4;

    uid->clock_seq_low = (clock_seq & 0xff);
    uid->clock_seq_hi_and_reserved = (clock_seq & 0x3f00) >> 8;
    uid->clock_seq_hi_and_reserved |= QUIDMAGIC;

    assert(memcpy(&uid->node, &node, sizeof(uid->node)));
    uid->node[0] = (uint8_t)true_random();
    uid->node[1] = QUID_REV4;
    uid->node[5] = (true_random() & 0xff);
}

/*
 * Format QUID from the timestamp, clocksequence, and node ID
 * Structure succeeds version 7 (REV7)
 */
static void format_quid_rev7(cuuid_t *uid, uint16_t clock_seq, cuuid_time_t timestamp) {
    uid->time_low = (uint64_t)(timestamp & 0xffffffff);
    uid->time_mid = (uint16_t)((timestamp >> 32) & 0xffff);

    uid->time_hi_and_version = (uint16_t)((timestamp >> 48) & 0xfff);
    uid->time_hi_and_version ^= QUIDMAGIC;
    uid->time_hi_and_version |= VERSION_REV7;

    uid->clock_seq_low = (clock_seq & 0xff);
    uid->clock_seq_hi_and_reserved = (clock_seq & 0x4e00) >> 8;
    uid->clock_seq_hi_and_reserved |= QUIDMAGIC;
}

/* Get current time including cpu clock */
static void get_current_time(cuuid_time_t *timestamp) {
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
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        assert(0);
    }

    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
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

    return ((uint16_t)(rand() + get_tick_count()));
}

/* Strip special characters from string */
static void strip_special_chars(char *s) {
    assert(s);
    char *pr = s, *pw = s;

    while (*pr) {
        *pw = *pr++;
        if (*pw != '-' && *pw != '{' && *pw != '}' && *pw != ' ') {
            pw++;
        }
    }

    *pw = '\0';
    assert(s);
}

/* Check if string validates as hex */
static int ishex(char *s) {
    while (*s) {
        if (!isxdigit(*s)) {
            return 0;
        }
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
    u->time_low = (uint64_t)strtoll(octet1, NULL, 16);
    assert(u->time_low != 0L && u->time_low != LLONG_MAX && u->time_low != LLONG_MIN);

    octet[0] = str[8];
    octet[1] = str[9];
    octet[2] = str[10];
    octet[3] = str[11];
    u->time_mid = (uint16_t)strtol(octet, NULL, 16);

    octet[0] = str[12];
    octet[1] = str[13];
    octet[2] = str[14];
    octet[3] = str[15];
    u->time_hi_and_version = (uint16_t)strtol(octet, NULL, 16);

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

/**
 * Validate input quid as genuine identifier by checking
 * its quid version and make sure some parts are non-zero.
 *
 * @param   cuuid  The quid to be validated in internal representation
 * @return         QUID_ERROR on faillure and QUID_OK on success
 */
QUID_LIB_API int quid_validate(cuuid_t *cuuid) {
    assert(cuuid);
    if ((cuuid->time_hi_and_version & VERSION_REV7) == VERSION_REV7) {
        cuuid->version = QUID_REV7;
    } else if ((cuuid->time_hi_and_version & VERSION_REV4) == VERSION_REV4) {
        cuuid->version = QUID_REV4;
    } else {
        return QUID_ERROR;
    }

    return QUID_OK;
}

/**
 * Convert string to quid identifier. If the string
 * cannot be parsed as a quid, and error is returned.
 *
 * @param    quid   Input string to be parsed by the function
 * @param    cuuid  Output quid structure provided by the caller
 * @return          QUID_ERROR on faillure and QUID_OK on success
 */
QUID_LIB_API int quid_parse(char *quid, cuuid_t *cuuid) {
    int len;
    assert(quid);

    /* Static size assert */
    SIZE_CHECK();

    /* Remove all special characters */
    strip_special_chars(quid);
    len = strlen(quid);

    /* Fail if invalid length */
    if (len != QUID_LEN) {
        return QUID_ERROR;
    }

    /* Fail if not hex */
    if (!ishex(quid)) {
        return QUID_ERROR;
    }

    /* Do the actual parsing */
    strtoquid(quid, cuuid);
    if (!quid_validate(cuuid)) {
        return QUID_ERROR;
    }

    return QUID_OK;
}

/**
 * Convert quid structure to string. The caller must provide
 * an array capable of holding the size of a QUID_FULLLEN+1. If
 * the str paramter does not contain enough elements or the parameter
 * is invalid memory, the resulting operation will cause an access violation.
 *
 * @param    cuuid  Input quid structure to be converted to string
 * @param    str    Output string in which the result will be written
 */
QUID_LIB_API void quid_tostring(const cuuid_t *cuuid, char str[QUID_FULLLEN + 1]) {
    assert(cuuid);
    snprintf(str, QUID_FULLLEN + 1, PRINT_QUID_FORMAT,
             cuuid->time_low,
             cuuid->time_mid,
             cuuid->time_hi_and_version,
             cuuid->clock_seq_hi_and_reserved,
             cuuid->clock_seq_low,
             cuuid->node[0],
             cuuid->node[1],
             cuuid->node[2],
             cuuid->node[3],
             cuuid->node[4],
             cuuid->node[5]);
}
