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

#ifndef __QUID__
#define __QUID__

#define UIDS_PER_TICK 1024
#define EPOCH_DIFF 11644473600LL
#define RANDFILE ".rnd"
#define MEM_SEED_CYCLE 65536
#define RND_SEED_CYCLE 4096
#define QUID_STRLEN 32
#define SEEDSZ 16

typedef struct {
	unsigned long time_low;
	unsigned short time_mid;
	unsigned short time_hi_and_version;
	unsigned char clock_seq_hi_and_reserved;
	unsigned char clock_seq_low;
	unsigned char node[6];
} cuuid_t;

typedef unsigned long long cuuid_time_t;

typedef struct {
	char nodeID[6];
} cuuid_node_t;

static void format_quid(cuuid_t *, unsigned short, cuuid_time_t, cuuid_node_t);
static void get_current_time(cuuid_time_t *);
static void get_mem_seed(cuuid_node_t *);
static void get_system_time(cuuid_time_t *);
static unsigned short true_random();
static double get_tick_count(void);
static unsigned short true_random(void);
int quid_create(cuuid_t *, char, char);
int quid_get_uid(char *, cuuid_t *);
void quid_set_rnd_seed(int);
void quid_set_mem_seed(int);
void strtouid(char *, cuuid_t *);

#endif
