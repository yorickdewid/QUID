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

#include <stdio.h>
#include <assert.h>
#include <quid.h>

void quid_print(const cuuid_t *u) {
    printf("{");

    printf("%.8x-", (unsigned int)u->time_low);
    printf("%.4x-", u->time_mid);
    printf("%.4x-", u->time_hi_and_version);
    printf("%x", u->clock_seq_hi_and_reserved);
    printf("%.2x-", u->clock_seq_low);

    printf("%.2x", u->node[0]);
    printf("%.2x", u->node[1]);
    printf("%.2x", u->node[2]);
    printf("%.2x", u->node[3]);
    printf("%.2x", u->node[4]);
    printf("%.2x", u->node[5]);

    printf("}\n");
}

int main(int argc, char *argv[]) {
    printf("Test vectors for QUID identifier\n");
    printf("=========================================\n\n");

    printf("TC1: General QUID.\n");
    printf("-------------------------\n");

    cuuid_t tc_1u;
    assert(quid_create(&tc_1u, IDF_NULL, CLS_CMON) == QUID_OK);
    quid_print(&tc_1u);

    printf("TC2: Single bit in key set. All zero IV.\n");
    printf("----------------------------------------\n");


    return 0;
}
