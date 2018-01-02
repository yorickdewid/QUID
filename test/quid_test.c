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
#include <limits.h>

#include <quid.h>

#include "tinytest.h"

static void lib_quid() {
	ASSERT("no version set", quid_libversion());
}

static void general_quid() {
	cuuid_t tc_u;
	int i;
	
	for (i = 0; i < 1000; ++i) {
		ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_NULL, CLS_CMON, NULL));
		ASSERT_EQUALS(QUID_OK, quid_validate(&tc_u));
	}
}

static void convert_string() {
	cuuid_t tc_u;
	char tc_str[QUID_FULLLEN + 1];

	ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_u));
	quid_tostring(&tc_u, tc_str);
	ASSERT("string does not match quid format", tc_str[0] == '{'
		   && tc_str[9] == '-'
		   && tc_str[14] == '-'
		   && tc_str[19] == '-'
		   && tc_str[QUID_FULLLEN - 1] == '}');
}

static void convert_string_and_back() {
	cuuid_t tc_3u, tc_3u_;
	char tc_3str[QUID_FULLLEN + 1];

	int i;
	for (i = 0; i < 20; ++i) {
		ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_3u));
		quid_tostring(&tc_3u, tc_3str);
		ASSERT("first char cannot be empty", tc_3str[0] != 0);
		ASSERT_EQUALS(QUID_OK, quid_parse(tc_3str, &tc_3u_));
		ASSERT("quid does not match", quid_cmp(&tc_3u, &tc_3u_));
	}
}

static void legacy_string_and_back() {
	cuuid_t tc_4u, tc_4u_;
	tc_4u.version = QUID_REV4;

	char tc_4str[QUID_FULLLEN + 1];
	ASSERT_EQUALS(QUID_OK, quid_create_simple(&tc_4u));
	quid_tostring(&tc_4u, tc_4str);
	ASSERT("first char cannot be empty", tc_4str[0] != 0);
	ASSERT_EQUALS(QUID_OK, quid_parse(tc_4str, &tc_4u_));
	ASSERT("quid does not match", quid_cmp(&tc_4u, &tc_4u_));
}

static void check_category_and_flags() {
	cuuid_t tc_u;

	ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_MASTER | IDF_STRICT, CLS_WARN, NULL));
	ASSERT("no flag found", quid_flag(&tc_u) & FLAG_MASTER);
	ASSERT("no flag found", quid_flag(&tc_u) & FLAG_STRICT);
	ASSERT_EQUALS(CLS_WARN, quid_category(&tc_u));
}

static void check_legacy_category_and_flags() {
	cuuid_t tc_u;
	tc_u.version = QUID_REV4;

	ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_MASTER | IDF_STRICT, CLS_WARN, NULL));
	ASSERT("no flag found", quid_flag(&tc_u) & FLAG_MASTER);
	ASSERT("no flag found", quid_flag(&tc_u) & FLAG_STRICT);
	ASSERT_EQUALS(CLS_WARN, quid_category(&tc_u));
}

static void check_tag() {
	cuuid_t tc_u;

	ASSERT_EQUALS(QUID_OK, quid_create(&tc_u, IDF_SIGNED | IDF_PUBLIC, CLS_ERROR, "CHK"));
	ASSERT("no flag found", quid_flag(&tc_u) & IDF_SIGNED);
	ASSERT("no flag found", quid_flag(&tc_u) & IDF_PUBLIC);
	ASSERT_EQUALS(CLS_ERROR, quid_category(&tc_u));
	ASSERT("string does not match", !strncmp(quid_tag(&tc_u), "CHK", 3));
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
	return TEST_REPORT();
}
