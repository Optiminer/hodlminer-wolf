#include <stdint.h>
#include <x86intrin.h>
#include "wolf-aes.h"

static inline void ExpandAESKey256_sub1(__m128i *tmp1, __m128i *tmp2)
{
    __m128i tmp4;
    *tmp2 = _mm_shuffle_epi32(*tmp2, 0xFF);
    tmp4 = _mm_slli_si128(*tmp1, 0x04);
    *tmp1 = _mm_xor_si128(*tmp1, tmp4);
    tmp4 = _mm_slli_si128(tmp4, 0x04);
    *tmp1 = _mm_xor_si128(*tmp1, tmp4);
    tmp4 = _mm_slli_si128(tmp4, 0x04);
    *tmp1 = _mm_xor_si128(*tmp1, tmp4);
    *tmp1 = _mm_xor_si128(*tmp1, *tmp2);
}

static inline void ExpandAESKey256_sub2(__m128i *tmp1, __m128i *tmp3)
{
    __m128i tmp2, tmp4;

    tmp4 = _mm_aeskeygenassist_si128(*tmp1, 0x00);
    tmp2 = _mm_shuffle_epi32(tmp4, 0xAA);
    tmp4 = _mm_slli_si128(*tmp3, 0x04);
    *tmp3 = _mm_xor_si128(*tmp3, tmp4);
    tmp4 = _mm_slli_si128(tmp4, 0x04);
    *tmp3 = _mm_xor_si128(*tmp3, tmp4);
    tmp4 = _mm_slli_si128(tmp4, 0x04);
    *tmp3 = _mm_xor_si128(*tmp3, tmp4);
    *tmp3 = _mm_xor_si128(*tmp3, tmp2);
}

// Special thanks to Intel for helping me
// with ExpandAESKey256() and its subroutines
void ExpandAESKey256(__m128i *keys, const __m128i *KeyBuf)
{
    __m128i tmp1, tmp2, tmp3;

    tmp1 = keys[0] = KeyBuf[0];
    tmp3 = keys[1] = KeyBuf[1];

    tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x01);
    ExpandAESKey256_sub1(&tmp1, &tmp2);
    keys[2] = tmp1;
    ExpandAESKey256_sub2(&tmp1, &tmp3);
    keys[3] = tmp3;

    tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x02);
    ExpandAESKey256_sub1(&tmp1, &tmp2);
    keys[4] = tmp1;
    ExpandAESKey256_sub2(&tmp1, &tmp3);
    keys[5] = tmp3;

    tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x04);
    ExpandAESKey256_sub1(&tmp1, &tmp2);
    keys[6] = tmp1;
    ExpandAESKey256_sub2(&tmp1, &tmp3);
    keys[7] = tmp3;

    tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x08);
    ExpandAESKey256_sub1(&tmp1, &tmp2);
    keys[8] = tmp1;
    ExpandAESKey256_sub2(&tmp1, &tmp3);
    keys[9] = tmp3;

    tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x10);
    ExpandAESKey256_sub1(&tmp1, &tmp2);
    keys[10] = tmp1;
    ExpandAESKey256_sub2(&tmp1, &tmp3);
    keys[11] = tmp3;

    tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x20);
    ExpandAESKey256_sub1(&tmp1, &tmp2);
    keys[12] = tmp1;
    ExpandAESKey256_sub2(&tmp1, &tmp3);
    keys[13] = tmp3;

    tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x40);
    ExpandAESKey256_sub1(&tmp1, &tmp2);
    keys[14] = tmp1;
}


static inline void AES256Core(__m128i* State, const __m128i ExpandedKey[][16])
{
    const uint32_t N = AES_PARALLEL_N;

    for(int j=0; j<N; ++j)
        State[j] = _mm_xor_si128(State[j], ExpandedKey[j][0]);

    for(int i = 1; i < 14; ++i)
        for(int j=0; j<N; ++j)
            State[j] = _mm_aesenc_si128(State[j], ExpandedKey[j][i]);

    for(int j=0; j<N; ++j)
        State[j] = _mm_aesenclast_si128(State[j], ExpandedKey[j][14]);
}

void AES256CBC(__m128i **Ciphertext, const __m128i **Plaintext, const __m128i ExpandedKey[][16], __m128i* IV, uint32_t BlockCount)
{

    const uint32_t N = AES_PARALLEL_N;
    __m128i State[N];
    for(int j=0; j<N; ++j)
        State[j] = _mm_xor_si128(Plaintext[j][0], IV[j]);

    AES256Core(State, ExpandedKey);
    for(int j=0; j<N; ++j)
        Ciphertext[j][0] = State[j];

    for(int i = 1; i < BlockCount; ++i)
    {
        for(int j=0; j<N; ++j)
            State[j] = _mm_xor_si128(Plaintext[j][i], Ciphertext[j][i - 1]);
                AES256Core(State, ExpandedKey);
                for(int j=0; j<N; ++j)
                Ciphertext[j][i] = State[j];
    }
}

