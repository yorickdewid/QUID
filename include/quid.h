/*
 * Copyright (c) 2012-2014, Yorick de Wid <tech at quenza dot net>
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

#define FLAG_PUBLIC 1<<0
#define FLAG_IDSAFE 1<<1
#define FLAG_MASTER 1<<2
#define FLAG_SIGNED 1<<3
#define FLAG_DMAGIC 1<<4
#define FLAG_TAGGED 1<<5
#define FLAG_STRICT 1<<6

#define IDF_NULL   0x00
#define IDF_PUBLIC 0x01
#define IDF_IDSAFE 0x02
#define IDF_MASTER 0x04
#define IDF_SIGNED 0x08
#define IDF_TAGGED 0x20
#define IDF_STRICT 0x40

#define CLS_CMON 0x1
#define CLS_INFO 0x2
#define CLS_WARN 0x3
#define CLS_ERROR 0x4

typedef struct {
	unsigned long   time_low;
	unsigned short  time_mid;
	unsigned short  time_hi_and_version;
	unsigned char   clock_seq_hi_and_reserved;
	unsigned char   clock_seq_low;
	unsigned char   node[6];
} cuuid_t;

extern int quid_create(cuuid_t *, char, char);
extern int quid_get_uid(char *, cuuid_t *);
extern void quid_print(cuuid_t, int);
extern void quid_print_file(FILE *, cuuid_t, int);
extern void quid_set_rnd_seed(int);
extern void quid_set_mem_seed(int);

#endif
