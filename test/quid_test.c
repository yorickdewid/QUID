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
    printf("%.2x",  u->clock_seq_hi_and_reserved);
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
    printf("----------------------------------------\n");

    cuuid_t tc_1u;
    assert(quid_create(&tc_1u, IDF_NULL, CLS_CMON, NULL) == QUID_OK);
    assert(quid_validate(&tc_1u) == QUID_OK);
    quid_print(&tc_1u);
    printf("\n");

    printf("TC2: Convert to string.\n");
    printf("----------------------------------------\n");

    cuuid_t tc_2u;
    char tc2_str[QUID_FULLLEN + 1];
    assert(quid_create_simple(&tc_2u) == QUID_OK);
    quid_tostring(&tc_2u, tc2_str);
    assert(tc2_str[0] != 0);
    puts(tc2_str);
    printf("\n");

    printf("TC3: Convert to string and back.\n");
    printf("----------------------------------------\n");

    cuuid_t tc_3u, tc_3u_;
    char tc_3str[QUID_FULLLEN + 1];
    assert(quid_create_simple(&tc_3u) == QUID_OK);
    quid_tostring(&tc_3u, tc_3str);
    assert(tc_3str[0] != 0);
    puts(tc_3str);
    assert(quid_parse(tc_3str, &tc_3u_) == QUID_OK);
    quid_print(&tc_3u);
    quid_print(&tc_3u_);
    assert(quid_cmp(&tc_3u, &tc_3u_));
    printf("\n");

    printf("TC4: Legacy version to string and back.\n");
    printf("----------------------------------------\n");

    cuuid_t tc_4u, tc_4u_;
    tc_4u.version = QUID_REV4;
    char tc_4str[QUID_FULLLEN + 1];
    assert(quid_create_simple(&tc_4u) == QUID_OK);
    quid_tostring(&tc_4u, tc_4str);
    assert(tc_4str[0] != 0);
    puts(tc_4str);
    assert(quid_parse(tc_4str, &tc_4u_) == QUID_OK);
    quid_print(&tc_4u);
    quid_print(&tc_4u_);
    assert(quid_cmp(&tc_4u, &tc_4u_));
    printf("\n");

    printf("TC5: Check category and flags.\n");
    printf("----------------------------------------\n");

    cuuid_t tc_5u;
    assert(quid_create(&tc_5u, IDF_MASTER | IDF_STRICT, CLS_WARN, NULL) == QUID_OK);
    quid_print(&tc_5u);
    assert(quid_flag(&tc_5u) & FLAG_MASTER);
    assert(quid_flag(&tc_5u) & FLAG_STRICT);
    assert(quid_category(&tc_5u) == CLS_WARN);
    printf("\n");

    printf("TC6: Check legacy category and flags.\n");
    printf("----------------------------------------\n");

    cuuid_t tc_6u;
    tc_6u.version = QUID_REV4;
    assert(quid_create(&tc_6u, IDF_MASTER | IDF_STRICT, CLS_WARN, NULL) == QUID_OK);
    quid_print(&tc_6u);
    assert(quid_flag(&tc_6u) & FLAG_MASTER);
    assert(quid_flag(&tc_6u) & FLAG_STRICT);
    assert(quid_category(&tc_6u) == CLS_WARN);
    printf("\n");

    printf("TC7: Check tag.\n");
    printf("----------------------------------------\n");

    cuuid_t tc_7u;
    assert(quid_create(&tc_7u, IDF_SIGNED | IDF_PUBLIC, CLS_ERROR, "CHK") == QUID_OK);
    quid_print(&tc_7u);
    assert(quid_flag(&tc_7u) & IDF_SIGNED);
    assert(quid_flag(&tc_7u) & IDF_PUBLIC);
    assert(quid_category(&tc_7u) == CLS_ERROR);
    assert(!strncmp(quid_tag(&tc_7u), "CHK", 3));
    printf("\n");

    return 0;
}
