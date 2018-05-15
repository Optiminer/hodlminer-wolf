#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <x86intrin.h>
#include <emmintrin.h>
#include "sha512.h"

#include "hodl.h"
#include "miner.h"
#include "wolf-aes.h"

void GenerateGarbageCore(CacheEntry *Garbage, int ThreadID, int ThreadCount, void *MidHash)
{
    uint64_t* TempBufs[SHA512_PARALLEL_N];
    uint64_t* desination[SHA512_PARALLEL_N];

    for (int i=0; i<SHA512_PARALLEL_N; ++i) {
        TempBufs[i] = (uint64_t*)malloc(32);
        memcpy(TempBufs[i], MidHash, 32);
    }

    uint32_t StartChunk = ThreadID * (TOTAL_CHUNKS / ThreadCount);
    for(uint32_t i = StartChunk; i < StartChunk + (TOTAL_CHUNKS / ThreadCount); i+= SHA512_PARALLEL_N) {
        for(int j=0; j<SHA512_PARALLEL_N; ++j) {
            ((uint32_t*)TempBufs[j])[0] = i + j;
            desination[j] = (uint64_t*)((uint8_t *)Garbage + ((i+j) * GARBAGE_CHUNK_SIZE));
        }
        sha512Compute32b_parallel(TempBufs, desination);
    }

    for (int i=0; i<SHA512_PARALLEL_N; ++i) {
        free(TempBufs[i]);
    }
}

void Rev256(uint32_t *Dest, const uint32_t *Src)
{
    for(int i = 0; i < 8; ++i) Dest[i] = swab32(Src[i]);
}

int scanhash_hodl(int threadNumber, int totalThreads, uint32_t *pdata, const CacheEntry *Garbage, const uint32_t *ptarget, unsigned long *hashes_done)
{
    uint32_t CollisionCount = 0;
    CacheEntry Cache[AES_PARALLEL_N];

    __m128i* data[AES_PARALLEL_N];
    __m128i* data0[AES_PARALLEL_N];
    const __m128i* next[AES_PARALLEL_N];

    for(int n=0; n<AES_PARALLEL_N; ++n) {
        data[n] = Cache[n].dqwords;
    }

    // Search for pattern in psuedorandom data
    int searchNumber = COMPARE_SIZE / totalThreads;
    int startLoc = threadNumber * searchNumber;

    for(int32_t k = startLoc; k < startLoc + searchNumber && !work_restart[threadNumber].restart; k+=AES_PARALLEL_N)
    {
        // Copy data
        for (int n=0; n<AES_PARALLEL_N; ++n) {
            memcpy(Cache[n].dwords, Garbage + k + n, GARBAGE_SLICE_SIZE);
        	//data0[n] = Garbage[k+n].dqwords;
        }

        for(int j = 0; j < AES_ITERATIONS; ++j)
        {
            __m128i ExpKey[AES_PARALLEL_N][16];
            __m128i ivs[AES_PARALLEL_N];

            // use last 4 bytes of first cache as next location
            for(int n=0; n<AES_PARALLEL_N; ++n) {
            	uint32_t nextLocation;
            	if (j == 0) {
            		nextLocation = Garbage[k+n].dwords[(GARBAGE_SLICE_SIZE >> 2) - 1] & (COMPARE_SIZE - 1); //% COMPARE_SIZE;
            	} else {
            		nextLocation = Cache[n].dwords[(GARBAGE_SLICE_SIZE >> 2) - 1] & (COMPARE_SIZE - 1); //% COMPARE_SIZE;
            	}
                next[n] = Garbage[nextLocation].dqwords;

                __m128i last[2];
                if (j == 0) {
                	last[0] = _mm_xor_si128(Garbage[k+n].dqwords[254], next[n][254]);
                	last[1] = _mm_xor_si128(Garbage[k+n].dqwords[255], next[n][255]);
                } else {
                	last[0] = _mm_xor_si128(Cache[n].dqwords[254], next[n][254]);
                	last[1] = _mm_xor_si128(Cache[n].dqwords[255], next[n][255]);
                }

                // Key is last 32b of Cache
                // IV is last 16b of Cache
                ExpandAESKey256(ExpKey[n], last);
                ivs[n] = last[1];
            }
            if (j == 0) {
            	AES256CBC(data, data, next, ExpKey, ivs);
            } else {
            	AES256CBC(data, data, next, ExpKey, ivs);
            }
        }

        // use last X bits as solution
        for(int n=0; n<AES_PARALLEL_N; ++n)
        if((Cache[n].dwords[(GARBAGE_SLICE_SIZE >> 2) - 1] & (COMPARE_SIZE - 1)) < 1000)
        {
            uint32_t BlockHdr[22], FinalPoW[8];

            BlockHdr[0] = swab32(pdata[0]);

            Rev256(BlockHdr + 1, pdata + 1);
            Rev256(BlockHdr + 9, pdata + 9);

            BlockHdr[17] = swab32(pdata[17]);
            BlockHdr[18] = swab32(pdata[18]);
            BlockHdr[19] = swab32(pdata[19]);
            BlockHdr[20] = k + n;
            BlockHdr[21] = Cache[n].dwords[(GARBAGE_SLICE_SIZE >> 2) - 2];

            sha256d((uint8_t *)FinalPoW, (uint8_t *)BlockHdr, 88);
            CollisionCount++;

            if(FinalPoW[7] <= ptarget[7])
            {
                pdata[20] = swab32(BlockHdr[20]);
                pdata[21] = swab32(BlockHdr[21]);
                *hashes_done = CollisionCount;
                return(1);
            }
        }
    }

    *hashes_done = CollisionCount;
    return(0);
}

void GenRandomGarbage(CacheEntry *Garbage, int totalThreads, uint32_t *pdata, int thr_id)
{
    uint32_t BlockHdr[20], MidHash[8];

    BlockHdr[0] = swab32(pdata[0]);

    Rev256(BlockHdr + 1, pdata + 1);
    Rev256(BlockHdr + 9, pdata + 9);

    BlockHdr[17] = swab32(pdata[17]);
    BlockHdr[18] = swab32(pdata[18]);
    BlockHdr[19] = swab32(pdata[19]);

    sha256d((uint8_t *)MidHash, (uint8_t *)BlockHdr, 80);

    GenerateGarbageCore(Garbage, thr_id, totalThreads, MidHash);
}
