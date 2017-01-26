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

#ifndef __QUID_H__
#define __QUID_H__

/*
 * Flags for individual identifiers
 * This provides extra informaton for the
 * recipient.
 *
 * Test flags using bitshit operators
 */
#define FLAG_PUBLIC 1<<0    /* Check for public flag */
#define FLAG_IDSAFE 1<<1    /* Check for safety flag */
#define FLAG_MASTER 1<<2    /* Check for master flag */
#define FLAG_SIGNED 1<<3    /* Check for signed flag */
#define FLAG_DMAGIC 1<<4    /* Check for magic flag verification */
#define FLAG_TAGGED 1<<5    /* Check for tagged flag */
#define FLAG_STRICT 1<<6    /* Check for strict flag */

#define IDF_NULL   0x00     /* Set no flag */
#define IDF_PUBLIC 0x01     /* Set flag to public */
#define IDF_IDSAFE 0x02     /* Set flag to safe */
#define IDF_MASTER 0x04     /* Set flag to master */
#define IDF_SIGNED 0x08     /* Set flag as signed */
#define IDF_TAGGED 0x20     /* Set flag as tag */
#define IDF_STRICT 0x40     /* Set flag to strict mode */

/*
 * Identifier classification
 * This provides extra informaton for the
 * recipient. Only one category can be of use
 * at a time.
 */
#define CLS_CMON 0x1    /* Set default class */
#define CLS_INFO 0x2    /* Set infomative class */
#define CLS_WARN 0x3    /* Set warning class */
#define CLS_ERROR 0x4   /* Set error class */

/*
 * Identifier structure
 */
typedef struct {
    unsigned long   time_low;           /* Time lover half */
    unsigned short  time_mid;           /* Time middle half */
    unsigned short  time_hi_and_version;        /* Time upper half and structure version */
    unsigned char   clock_seq_hi_and_reserved;  /* Clock sequence */
    unsigned char   clock_seq_low;          /* Clock sequence lower half */
    unsigned char   node[6];            /* Node allocation, filled with random memory data */
} cuuid_t;

/*
 * Prototypes to library functions
 */

#if defined(__cplusplus)
extern "c" {
#endif

extern int quid_create(cuuid_t *, char, char);
extern int quid_get_uid(char *, cuuid_t *);
extern void quid_print(cuuid_t, int);
extern void quid_print_file(FILE *, cuuid_t, int);
extern void quid_set_rnd_seed(int);
extern void quid_set_mem_seed(int);
extern char *quid_libversion(void);

#if defined(__cplusplus)
}
#endif

#endif // __QUID_H__
