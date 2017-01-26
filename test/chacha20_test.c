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
#include "../src/chacha20.h"

void print_block(uint8_t block[64]) {
    uint8_t i;

    for (i = 0 ; i < 64 ; i++) {
        printf("0x%02x ", block[i]);

        if (((i + 1) % 8) == 0) {
            printf("\n");
        }
    }

    printf("\n");
}

void print_key_iv(uint8_t *key, uint32_t keylen,  uint8_t *iv) {
    uint8_t i;

    printf("Key:    ");
    for (i = 0 ; i < (keylen / 8) ; i++) {
        if ((i > 0) && (0 == (i % 8))) {
            printf("\n        ");
        }
        printf("0x%02x ", key[i]);
    }
    printf("\n");

    printf("IV:     ");
    for (i = 0 ; i < 8 ; i++) {
        printf("0x%02x ", iv[i]);
    }
    printf("\n");
}

void gen_testvectors(uint8_t *key, uint8_t *iv) {
    uint32_t keylengths[2] = {128, 256};
    uint8_t rounds[4] = {8, 12, 20};

    uint8_t data[64] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t result0[64] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t result1[64] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint32_t ki;
    uint8_t ri;
    chacha_ctx my_ctx;

    /* For a given key and iv we process two consecutive blocks
    * using 8, 12 or 20 rounds */
    for (ki = 0 ; ki < 2 ; ki++) {
        for (ri = 0 ; ri < 3 ; ri++) {
            print_key_iv(key, keylengths[ki], iv);
            printf("Rounds: %d\n\n", rounds[ri]);

            chacha_init_ctx(&my_ctx, rounds[ri]);
            chacha_init(&my_ctx, key, keylengths[ki], iv);

            chacha_next(&my_ctx, data, result0);
            chacha_next(&my_ctx, data, result1);

            printf("Keystream block 0:\n");
            print_block(result0);
            printf("Keystream block 1:\n");
            print_block(result1);
            printf("\n");
        }
    }
}

int main(void) {
    printf("Test vectors for the ChaCha stream cipher\n");
    printf("=========================================\n\n");


    printf("TC1: All zero key and IV.\n");
    printf("-------------------------\n");
    uint8_t tc1_key[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t tc1_iv[8]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    gen_testvectors(tc1_key, tc1_iv);
    printf("\n");

return 0;
    printf("TC2: Single bit in key set. All zero IV.\n");
    printf("----------------------------------------\n");
    uint8_t tc2_key[32] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t tc2_iv[8]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    gen_testvectors(tc2_key, tc2_iv);
    printf("\n");


    printf("TC3: Single bit in IV set. All zero key.\n");
    printf("----------------------------------------\n");
    uint8_t tc3_key[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t tc3_iv[8]   = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    gen_testvectors(tc3_key, tc3_iv);
    printf("\n");


    printf("TC4: All bits in key and IV are set.\n");
    printf("------------------------------------\n");
    uint8_t tc4_key[32] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t tc4_iv[8]   = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    gen_testvectors(tc4_key, tc4_iv);
    printf("\n");


    printf("TC5: Every even bit set in key and IV.\n");
    printf("--------------------------------------\n");
    uint8_t tc5_key[32] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                         0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                         0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                         0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    uint8_t tc5_iv[8]   = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    gen_testvectors(tc5_key, tc5_iv);
    printf("\n");


    printf("TC6: Every odd bit set in key and IV.\n");
    printf("-------------------------------------\n");
    uint8_t tc6_key[32] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                         0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                         0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                         0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
    uint8_t tc6_iv[8]   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
    gen_testvectors(tc6_key, tc6_iv);
    printf("\n");


    // TC4: Sequence patterns.
    printf("TC7: Sequence patterns in key and IV.\n");
    printf("-------------------------------------\n");
    uint8_t tc7_key[32] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                         0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
                         0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
    uint8_t tc7_iv[8]   = {0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78};
    gen_testvectors(tc7_key, tc7_iv);
    printf("\n");


    // TC 8: A random key and IV.
    // key: echo -n "All your base are belong to us" | openssl dgst -sha256
    // IV: echo -n "Internet Engineering Task Force" | openssl dgst -sha256
    printf("TC8: key: 'All your base are belong to us!, IV: 'IETF2013'\n");
    printf("----------------------------------------------------------\n");
    uint8_t tc8_key[32] = {0xc4, 0x6e, 0xc1, 0xb1, 0x8c, 0xe8, 0xa8, 0x78,
                         0x72, 0x5a, 0x37, 0xe7, 0x80, 0xdf, 0xb7, 0x35,
                         0x1f, 0x68, 0xed, 0x2e, 0x19, 0x4c, 0x79, 0xfb,
                         0xc6, 0xae, 0xbe, 0xe1, 0xa6, 0x67, 0x97, 0x5d};
    uint8_t tc8_iv[8]   = {0x1a, 0xda, 0x31, 0xd5, 0xcf, 0x68, 0x82, 0x21};
    gen_testvectors(tc8_key, tc8_iv);

  return 0;
}
