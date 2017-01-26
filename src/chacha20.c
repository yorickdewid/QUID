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

#include <stdlib.h>
#include "chacha20.h"

/* Basic 32-bit operators */
#define ROTATE(v,c) ((uint32_t)((v) << (c)) | ((v) >> (32 - (c))))
#define XOR(v,w) ((v) ^ (w))
#define PLUS(v,w) ((uint32_t)((v) + (w)))
#define PLUSONE(v) (PLUS((v), 1))

/* Little endian machine assumed (x86-64) */
#define U32TO8_LITTLE(p, v) (((uint32_t*)(p))[0] = v)
#define U8TO32_LITTLE(p) (((uint32_t*)(p))[0])

#define QUARTERROUND(a, b, c, d) \
    x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
    x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
    x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
    x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);

static const uint8_t SIGMA[16] = "expand 32-byte k";
static const uint8_t TAU[16]   = "expand 16-byte k";

static void doublerounds(uint8_t output[64], const uint32_t input[16], uint8_t rounds) {
    uint32_t x[16];
    int32_t i;

    for (i = 0;i < 16;++i) {
        x[i] = input[i];
    }

    for (i = rounds ; i > 0 ; i -= 2) {
        QUARTERROUND( 0, 4, 8,12)
        QUARTERROUND( 1, 5, 9,13)
        QUARTERROUND( 2, 6,10,14)
        QUARTERROUND( 3, 7,11,15)

        QUARTERROUND( 0, 5,10,15)
        QUARTERROUND( 1, 6,11,12)
        QUARTERROUND( 2, 7, 8,13)
        QUARTERROUND( 3, 4, 9,14)
    }

    for (i = 0;i < 16;++i) {
        x[i] = PLUS(x[i], input[i]);
    }

    for (i = 0;i < 16;++i) {
        U32TO8_LITTLE(output + 4 * i, x[i]);
    }
}

void chacha_init(chacha_ctx *x, uint8_t *key, uint32_t keylen, uint8_t *iv) {
    switch (keylen) {
        case 256:
            x->state[0]  = U8TO32_LITTLE(SIGMA + 0);
            x->state[1]  = U8TO32_LITTLE(SIGMA + 4);
            x->state[2]  = U8TO32_LITTLE(SIGMA + 8);
            x->state[3]  = U8TO32_LITTLE(SIGMA + 12);
            x->state[4]  = U8TO32_LITTLE(key + 0);
            x->state[5]  = U8TO32_LITTLE(key + 4);
            x->state[6]  = U8TO32_LITTLE(key + 8);
            x->state[7]  = U8TO32_LITTLE(key + 12);
            x->state[8]  = U8TO32_LITTLE(key + 16);
            x->state[9]  = U8TO32_LITTLE(key + 20);
            x->state[10] = U8TO32_LITTLE(key + 24);
            x->state[11] = U8TO32_LITTLE(key + 28);
            break;
        case 128:
            x->state[0]  = U8TO32_LITTLE(TAU + 0);
            x->state[1]  = U8TO32_LITTLE(TAU + 4);
            x->state[2]  = U8TO32_LITTLE(TAU + 8);
            x->state[3]  = U8TO32_LITTLE(TAU + 12);
            x->state[4]  = U8TO32_LITTLE(key + 0);
            x->state[5]  = U8TO32_LITTLE(key + 4);
            x->state[6]  = U8TO32_LITTLE(key + 8);
            x->state[7]  = U8TO32_LITTLE(key + 12);
            x->state[8]  = U8TO32_LITTLE(key + 0);
            x->state[9]  = U8TO32_LITTLE(key + 4);
            x->state[10] = U8TO32_LITTLE(key + 8);
            x->state[11] = U8TO32_LITTLE(key + 12);
            break;
        default:
            abort();
    }

    /* Reset block counter and add IV to state */
    x->state[12] = 0;
    x->state[13] = 0;
    x->state[14] = U8TO32_LITTLE(iv + 0);
    x->state[15] = U8TO32_LITTLE(iv + 4);
}

void chacha_next(chacha_ctx *ctx, const uint8_t *m, uint8_t *c) {
    uint8_t x[64];
    uint8_t i;

    /* Update the internal state and increase the block counter */
    doublerounds(x, ctx->state, ctx->rounds);
    ctx->state[12] = PLUSONE(ctx->state[12]);
    if (!ctx->state[12]) {
        ctx->state[13] = PLUSONE(ctx->state[13]);
    }

    /* XOR the input block with the new temporal state to
     * create the transformed block */
    for (i = 0 ; i < 64 ; ++i) {
        c[i] = m[i] ^ x[i];
    }
}

void chacha_init_ctx(chacha_ctx *ctx, uint8_t rounds) {
    uint8_t i;

    for (i = 0 ; i < 16 ; i++) {
        ctx->state[i] = 0;
    }

    // memset()

    ctx->rounds = rounds;
}
