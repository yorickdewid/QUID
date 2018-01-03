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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <quid.h>

#include "tinytest.h"

#ifdef _WIN32
# define STRCOPY(s,c) strcpy_s(s, sizeof(s), c);
#else
# define STRCOPY(s,c) strcpy(s, c);
#endif

static void lib_quid() {
    ASSERT("no version set", quid_libversion());
}

static void general_quid() {
    cuuid_t tc_u;
    int i;
    
    for (i = 0; i < 10000; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_NULL, CLS_CMON, NULL));
        ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
    }

    quid_set_rnd_seed(2);
    
    for (i = 0; i < 5000; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_NULL, CLS_CMON, NULL));
        ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
    }

    quid_set_rnd_seed(1 << 20);

    for (i = 0; i < 5000; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_NULL, CLS_CMON, NULL));
        ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
    }
}

static void convert_string() {
    cuuid_t tc_u;
    char tc_str[QUID_FULLLEN + 1];
    int i;

    for (i = 0; i < 100; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_u));
        quid_tostring(&tc_u, tc_str);
        ASSERT("string does not match quid format", tc_str[0] == '{'
               && tc_str[9] == '-'
               && tc_str[14] == '-'
               && tc_str[19] == '-'
               && tc_str[QUID_FULLLEN - 1] == '}');
    }
}

static void convert_string_and_back() {
    cuuid_t tc_u, tc_b, tc_c;
    char tc_str[QUID_FULLLEN + 1];
    int i;

    for (i = 0; i < 20; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_u));
        quid_tostring(&tc_u, tc_str);
        ASSERT("first char cannot be empty", tc_str[0] != 0);
        ASSERT_EQUALS(QUID_OK, quid_parse(tc_str, &tc_b));
        ASSERT("quid does not match", quid_cmp(&tc_u, &tc_b));
    }

    STRCOPY(tc_str, "{faf38af0-c099-b089-ca43-75cfd0bb5725}");
    ASSERT_EQUALS(QUID_OK, quid_parse(tc_str, &tc_c));
    STRCOPY(tc_str, "b4be7a9e-ca82-b0b5-8000-9a3df5bf6f88");
    ASSERT_EQUALS(QUID_OK, quid_parse(tc_str, &tc_c));
}

//TODO
static void legacy_string_and_back() {
    cuuid_t tc_4u, tc_4u_;
    char tc_4str[QUID_FULLLEN + 1];
    tc_4u.version = QUID_REV4;

    ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_4u));
    quid_tostring(&tc_4u, tc_4str);
    ASSERT("first char cannot be empty", tc_4str[0] != 0);
    ASSERT_EQUALS(QUID_OK, quid_parse(tc_4str, &tc_4u_));
    ASSERT("quid does not match", quid_cmp(&tc_4u, &tc_4u_));
}

static void check_category_and_flags() {
    cuuid_t tc_u;
    int i;

    for (i = 0; i < 2000; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_MASTER | IDF_STRICT, CLS_WARN, NULL));
        ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
        ASSERT("no flag found", quid_flag(&tc_u) & FLAG_MASTER);
        ASSERT("no flag found", quid_flag(&tc_u) & FLAG_STRICT);
        ASSERT_EQUALS(CLS_WARN, quid_category(&tc_u));
    }
}

//TODO
static void check_legacy_category_and_flags() {
    cuuid_t tc_u;
    tc_u.version = QUID_REV4;

    ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_MASTER | IDF_STRICT, CLS_WARN, NULL));
    ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
    ASSERT("no flag found", quid_flag(&tc_u) & FLAG_MASTER);
    ASSERT("no flag found", quid_flag(&tc_u) & FLAG_STRICT);
    ASSERT_EQUALS(CLS_WARN, quid_category(&tc_u));
}

static void check_tag() {
    cuuid_t tc_u;
    int i;

    for (i = 0; i < 100; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_SIGNED | IDF_PUBLIC, CLS_ERROR, "CHK"));
        ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
        ASSERT("no flag found", quid_flag(&tc_u) & IDF_SIGNED);
        ASSERT("no flag found", quid_flag(&tc_u) & IDF_PUBLIC);
        ASSERT_EQUALS(CLS_ERROR, quid_category(&tc_u));
        ASSERT("string does not match", !strncmp(quid_tag(&tc_u), "CHK", 3));
    }
}

static void check_timestamp() {
    cuuid_t tc_u;
    int i;

    for (i = 0; i < 500; ++i) {
        time_t timt = time(NULL);
        struct tm ti1;
#ifdef _WIN32
        assert(gmtime_s(&ti1, &timt) == 0);
#else
        struct tm *_ti1 = gmtime(&timt);
        ti1 = *_ti1;
#endif
        ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_u));
        ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
        struct tm *ti2 = quid_timestamp(&tc_u);
        assert(ti2);
        ASSERT("timestamp skew", ti1.tm_year == ti2->tm_year
               && ti1.tm_mon == ti2->tm_mon
               && ti1.tm_mday == ti2->tm_mday
               && ti1.tm_hour == ti2->tm_hour
               && ti1.tm_yday == ti2->tm_yday
               && ti1.tm_min == ti2->tm_min
               && ti1.tm_sec == ti2->tm_sec);

        ASSERT("zero microtime is possible, but unlikely", quid_microtime(&tc_u) > 0);
    }
}

//TODO:
static void check_quid_version() {
    cuuid_t tc_u;
    int i;

    for (i = 0; i < 5000; ++i) {
        ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_u));
        ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
    }
}

int main() {
    printf("Test vectors for QUID identifier\n");
    printf("=========================================\n\n");

    RUN(lib_quid);
    RUN(general_quid);
    RUN(convert_string);
    RUN(convert_string_and_back);
    RUN(legacy_string_and_back);
    RUN(check_category_and_flags);
    RUN(check_legacy_category_and_flags);
    RUN(check_tag);
    RUN(check_timestamp);
    RUN(check_quid_version);
    return TEST_REPORT();
}
