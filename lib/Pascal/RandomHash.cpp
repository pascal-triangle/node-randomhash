/**
 * RandomHash source code implementation
 *
 * Copyright 2018 Polyminer1 <https://github.com/polyminer1>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
///
/// @file
/// @copyright Polyminer1

#include <memory.h>
#include <stdint.h>

#include "utils.h"
#include "basetypes.h"

#include "RandomHash.h"
#include "RandomHash_MurMur3_32.h"  
#include "RandomHash_Blake2b.h" 
#include "RandomHash_Blake2s.h"
#include "RandomHash_Grindahl512.h"
#include "RandomHash_Haval_5_256.h" 
#include "RandomHash_MD5.h"
#include "RandomHash_RadioGatun32.h"
#include "RandomHash_RIPEMD160.h" 
#include "RandomHash_RIPEMD256.h"
#include "RandomHash_RIPEMD320.h"
#include "RandomHash_SHA2_256.h"
#include "RandomHash_SHA2_512.h"
#include "RandomHash_SHA3_512.h"
#include "RandomHash_Snefru_8_256.h"
#include "RandomHash_Tiger2_5_192.h"
#include "RandomHash_Whirlpool.h"


struct RH_StrideArrayStruct
{
    U32 size;
    U32 maxSize;
    U8  dummy[(RH_IDEAL_ALIGNMENT/2) - 8];
    MurmurHash3_x86_32_State accum;
    U8  dummy2[(RH_IDEAL_ALIGNMENT/2) - sizeof(MurmurHash3_x86_32_State)];
    U8* strides[RH_StrideArrayCount];
};

#define RH_StrideArrayStruct_GetAccum(strideArray) (&((RH_StrideArrayStruct*)strideArray)->accum)

const size_t c_round_outputsCounts[6] = {0, 1, 3, 7, 15, 31 };
const size_t c_round_parenAndNeighbortOutputsCounts[6] = { 31, 0, 1, 3, 7, 15 };

inline RH_StridePtr RH_StrideArrayGet(RH_StridePtrArray strideArrayVar, int idx)
{
    RH_ASSERT(idx <= (int)RH_STRIDEARRAY_GET_MAXSIZE(strideArrayVar));
    return ((RH_StrideArrayStruct*)strideArrayVar)->strides[idx];
}

inline RH_StridePtr RH_StrideArrayAllocOutput(RandomHash_State* state)
{
    RH_StridePtr stride = ((U8*)state->m_stridesInstances) + state->m_stridesAllocIndex*RH_StrideSize;

    state->m_stridesAllocIndex++;

    if (!(state->m_stridesAllocIndex < RH_TOTAL_STRIDES_INSTANCES))
	exit(-(83));

    if (!((((size_t)stride) % 32) == 0))
	exit(-(84));

    memset(stride, 0, RH_StrideSize);
    RH_STRIDE_INIT(stride);

    return stride;
}


inline void RH_StrideArrayReset(RH_StridePtrArray strideArrayVar)
{
    RH_StrideArrayStruct* arr = (RH_StrideArrayStruct*)strideArrayVar;

    arr->size = 0;
    static_assert(sizeof(MurmurHash3_x86_32_State) == 2 * sizeof(U64), "Incorrect struct size");
    U64* accum = (U64*)&arr->accum;
    accum[0] = 0;
    accum[1] = 0;

    static_assert(sizeof(void*) == sizeof(U64), "Incorrect ptr size");
    memset(arr->strides, 0, RH_StrideArrayCount * sizeof(void*));
}


#include "RandomHash_inl.h"


inline U32 GetNextRnd(mersenne_twister_state* gen)
{
    return merssen_twister_rand(gen);
}

void RandomHash_RoundDataInit(RH_RoundData* rd, int round)
{

    if (c_round_outputsCounts[round])
        RH_STRIDEARRAY_RESET(rd->roundOutputs);

    if (c_round_parenAndNeighbortOutputsCounts[round])
        RH_STRIDEARRAY_RESET(rd->parenAndNeighbortOutputs);

    if (rd->otherNonceHeader && round > 0)
        RH_STRIDE_INIT(rd->otherNonceHeader);

    RH_STRIDE_INIT(rd->roundInput);
}

inline void RandomHash_Initialize(RandomHash_State* state)
{
    RandomHash_RoundDataInit(&state->m_data[0], 0);
    RandomHash_RoundDataInit(&state->m_data[1], 1);
    RandomHash_RoundDataInit(&state->m_data[2], 2);
    RandomHash_RoundDataInit(&state->m_data[3], 3);
    RandomHash_RoundDataInit(&state->m_data[4], 4);
    RandomHash_RoundDataInit(&state->m_data[5], 5);

    state->m_stridesAllocIndex = 0;
}

void RandomHash_Destroy(RandomHash_State* state)
{
    RandomHash_RoundDataUnInit(&state->m_data[0], 0);
    RandomHash_RoundDataUnInit(&state->m_data[1], 1);
    RandomHash_RoundDataUnInit(&state->m_data[2], 2);
    RandomHash_RoundDataUnInit(&state->m_data[3], 3);
    RandomHash_RoundDataUnInit(&state->m_data[4], 4);
    RandomHash_RoundDataUnInit(&state->m_data[5], 5);

    RandomHash_Free(state->m_cachedOutputs);
    RandomHash_Free(state->m_cachedHheader);

    RandomHash_Free(state->m_stridesInstances);
}

void AllocateArray(U8*& arrayData, int count)
{
    U32 size = sizeof(RH_StrideArrayStruct);
    RandomHash_Alloc((void**)&arrayData, size);
    memset(arrayData, 0, size);

    RH_STRIDEARRAY_GET_MAXSIZE(arrayData) = count;
    RH_STRIDEARRAY_INIT(arrayData);

}

void RandomHash_RoundDataAlloc(RH_RoundData* rd, int round)
{
    memset(rd, 0, sizeof(RH_RoundData));

    if (c_round_outputsCounts[round] > 0)
        AllocateArray(rd->roundOutputs, c_round_outputsCounts[round]);

    if (c_round_parenAndNeighbortOutputsCounts[round])
        AllocateArray(rd->parenAndNeighbortOutputs, c_round_parenAndNeighbortOutputsCounts[round]);

    RandomHash_Alloc((void**)&rd->roundInput, 208896);

    if (round > 0)
        RandomHash_Alloc((void**)&rd->otherNonceHeader, 208896);

}

void RandomHash_Create(RandomHash_State* state)
{
    RandomHash_Alloc((void**)&state->m_stridesInstances, 208896 * ((31 +1)*2));

    state->m_stridesAllocIndex = 0;

    RandomHash_RoundDataAlloc(&state->m_data[0], 0);
    RandomHash_RoundDataAlloc(&state->m_data[1], 1);
    RandomHash_RoundDataAlloc(&state->m_data[2], 2);
    RandomHash_RoundDataAlloc(&state->m_data[3], 3);
    RandomHash_RoundDataAlloc(&state->m_data[4], 4);
    RandomHash_RoundDataAlloc(&state->m_data[5], 5);

    RandomHash_Alloc((void**)&state->m_cachedHheader, PascalHeaderSize);
    memset(state->m_cachedHheader, 0, PascalHeaderSize);

    AllocateArray(state->m_cachedOutputs, c_round_outputsCounts[5]);

    RandomHash_Initialize(state);
}

void RandomHash_RoundDataUnInit(RH_RoundData* rd, int round)
{
    RandomHash_Free(rd->roundOutputs);
    RandomHash_Free(rd->parenAndNeighbortOutputs);
    RandomHash_Free(rd->roundInput);
    RandomHash_Free(rd->otherNonceHeader);
}


inline void RandomHash_Reseed(mersenne_twister_state& rndGen, U32 seed)
{
    merssen_twister_seed(seed, &rndGen);
}


inline U32 RandomHash_Checksum(RH_StridePtr input)
{
    U32 csum = 0;
    csum = MurmurHash3_x86_32_Fast((const void *)RH_STRIDE_GET_DATA(input), RH_STRIDE_GET_SIZE(input), 0);
    return csum;
}

U32 RandomHash_ChecksumArray(RH_StridePtrArray inputs)
{
    U32 csum;
    MurmurHash3_x86_32_State state;
    MurmurHash3_x86_32_Init(0, &state);

    RH_STRIDEARRAY_FOR_EACH_BEGIN(inputs)
    {
        MurmurHash3_x86_32_Update(RH_STRIDE_GET_DATA(strideItrator), RH_STRIDE_GET_SIZE(strideItrator), &state);
    }
    RH_STRIDEARRAY_FOR_EACH_END(inputs)
	csum = MurmurHash3_x86_32_Finalize(&state);
    return csum;
}

void RandomHash_Expand(RandomHash_State* state, RH_StridePtr input, int round, int ExpansionFactor )
{
    U32 inputSize = RH_STRIDE_GET_SIZE(input);
    U32 seed = RandomHash_Checksum(input);
    RandomHash_Reseed(state->m_rndGenExpand, seed);
    size_t sizeExp = inputSize + ExpansionFactor * (((10)*1024LLU)*5);

    RH_StridePtr output = input;

    S64 bytesToAdd = sizeExp - inputSize;

    U8* outputPtr = RH_STRIDE_GET_DATA(output);
    while (bytesToAdd > 0)
    {
        U32 nextChunkSize = RH_STRIDE_GET_SIZE(output);

        U8* nextChunk = outputPtr + nextChunkSize;
        if (nextChunkSize > bytesToAdd)
            nextChunkSize = (U32)bytesToAdd;

        U32 random = GetNextRnd(&state->m_rndGenExpand);
        U32 r = random % 8;

	switch(r)
	{
	case 0: Transfo0_2(nextChunk, nextChunkSize, outputPtr); break;
	case 1: Transfo1_2(nextChunk, nextChunkSize, outputPtr); break;
	case 2: Transfo2_2(nextChunk, nextChunkSize, outputPtr); break;
	case 3: Transfo3_2(nextChunk, nextChunkSize, outputPtr); break;
	case 4: Transfo4_2(nextChunk, nextChunkSize, outputPtr); break;
	case 5: Transfo5_2(nextChunk, nextChunkSize, outputPtr); break;
	case 6: Transfo6_2(nextChunk, nextChunkSize, outputPtr); break;
	case 7: Transfo7_2(nextChunk, nextChunkSize, outputPtr); break;
	}

        RH_STRIDE_GET_SIZE(output) += nextChunkSize;

        bytesToAdd = bytesToAdd - nextChunkSize;
    }

   
    RH_STRIDE_CHECK_INTEGRITY(output);
}

void RandomHash_Compress(RandomHash_State* state, RH_StridePtrArray inputs, RH_StridePtr Result)
{
    U32 rval;

    MurmurHash3_x86_32_State tstate = *(&((RH_StrideArrayStruct*)inputs)->accum);
    U32 seed = MurmurHash3_x86_32_Finalize(&tstate);

    RandomHash_Reseed(state->m_rndGenCompress, seed);

    RH_STRIDE_SET_SIZE(Result, 100);
    U8* resultPtr = RH_STRIDE_GET_DATA(Result);
    U32 inoutSize = RH_STRIDEARRAY_GET_SIZE(inputs);

    for (size_t i = 0; i < 100; i++)
    {
        RH_StridePtr source = RH_STRIDEARRAY_GET(inputs, GetNextRnd(&state->m_rndGenCompress) % inoutSize);
        U32 sourceSize = RH_STRIDE_GET_SIZE(source);

        rval = GetNextRnd(&state->m_rndGenCompress);
        resultPtr[i] = RH_STRIDE_GET_DATA(source)[rval % sourceSize];
    }

    RH_STRIDE_CHECK_INTEGRITY(Result);
}

inline void RandomHash_start(RandomHash_State* state, U32 in_round)
{
    U32 seed = RandomHash_Checksum(state->m_data[in_round].in_blockHeader);
    RandomHash_Reseed(state->m_data[in_round].rndGen, seed);

    RH_STRIDE_COPY(state->m_data[in_round].roundInput, state->m_data[in_round].in_blockHeader);
}

inline void RandomHash_Phase_1_push(RandomHash_State* state, int in_round)
{

    state->m_data[in_round-1].backup_in_blockHeader = state->m_data[in_round-1].in_blockHeader;
    state->m_data[in_round-1].in_blockHeader = state->m_data[in_round].in_blockHeader;
    state->m_data[in_round-1].backup_io_results = state->m_data[in_round-1].io_results;
    state->m_data[in_round-1].io_results = state->m_data[in_round].parenAndNeighbortOutputs;
}

inline void RandomHash_Phase_1_pop(RandomHash_State* state, int in_round)
{
    if (in_round == 5 && state->m_skipPhase1)
    {
        RH_STRIDEARRAY_CLONE(state->m_data[5].parenAndNeighbortOutputs, state->m_cachedOutputs, state);

        RH_STRIDEARRAY_CHECK_INTEGRITY(state->m_cachedOutputs);
        RH_STRIDEARRAY_RESET(state->m_cachedOutputs);
        state->m_skipPhase1 = 0;
    }
    else
    {
        state->m_data[in_round-1].in_blockHeader = state->m_data[in_round-1].backup_in_blockHeader;
        state->m_data[in_round-1].io_results = state->m_data[in_round-1].backup_io_results;
    }

    MurmurHash3_x86_32_State tstate = *(&((RH_StrideArrayStruct*)state->m_data[in_round].parenAndNeighbortOutputs)->accum);
    U32 seed = MurmurHash3_x86_32_Finalize(&tstate);

    RandomHash_Reseed(state->m_data[in_round].rndGen, seed);

    RH_STRIDEARRAY_PUSHBACK_MANY(state->m_data[in_round].roundOutputs, state->m_data[in_round].parenAndNeighbortOutputs);

    RH_STRIDEARRAY_RESET(state->m_data[in_round].parenAndNeighbortOutputs);
}

inline void RandomHash_Phase_2_push(RandomHash_State* state, int in_round)
{
    RH_STRIDE_COPY(state->m_data[in_round].otherNonceHeader, state->m_data[in_round].in_blockHeader);
    U32 newNonce = GetNextRnd(&state->m_data[in_round].rndGen);

    *(U32*)(RH_STRIDE_GET_DATA(state->m_data[in_round].otherNonceHeader)+(PascalHeaderSize - 4)) = newNonce;

    state->m_data[in_round-1].backup_in_blockHeader = state->m_data[in_round-1].in_blockHeader;
    state->m_data[in_round-1].in_blockHeader = state->m_data[in_round].otherNonceHeader;
    state->m_data[in_round-1].backup_io_results = state->m_data[in_round-1].io_results;
    state->m_data[in_round-1].io_results = state->m_data[in_round].parenAndNeighbortOutputs;
}


void RandomHash_Phase_2_pop(RandomHash_State* state, int in_round)
{
    state->m_data[in_round-1].in_blockHeader = state->m_data[in_round-1].backup_in_blockHeader;
    state->m_data[in_round-1].io_results = state->m_data[in_round-1].backup_io_results;

    RH_STRIDEARRAY_PUSHBACK_MANY(state->m_data[in_round].roundOutputs, state->m_data[in_round].parenAndNeighbortOutputs);

    RandomHash_Compress(state, state->m_data[in_round].roundOutputs, state->m_data[in_round].roundInput);

    RH_STRIDEARRAY_RESET(state->m_data[in_round].parenAndNeighbortOutputs);
}

inline void RandomHash_Phase_init(RandomHash_State* state, int in_round)
{
    RH_STRIDEARRAY_RESET(state->m_data[in_round].roundOutputs);
    RH_STRIDE_RESET(state->m_data[in_round].roundInput);
}


inline void RandomHash(RandomHash_State* state, int in_round)
{
    RH_StridePtr output = RH_StrideArrayAllocOutput(state);
    RH_STRIDEARRAY_PUSHBACK(state->m_data[in_round].roundOutputs, output);

    U32 rndHash = GetNextRnd(&state->m_data[in_round].rndGen) % 18;

    switch(rndHash)
    {
    case RandomHashAlgos::RH_SHA2_256 :
    {
	RandomHash_SHA2_256(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_SHA2_384 :
    {
	RandomHash_SHA2_384(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_SHA2_512 :
    {
	RandomHash_SHA2_512(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_SHA3_256 :
    {
	RandomHash_SHA3_256(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_SHA3_384 :
    {
	RandomHash_SHA3_384(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_SHA3_512 :
    {
	RandomHash_SHA3_512(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_RIPEMD160 :
    {
	RandomHash_RIPEMD160(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_RIPEMD256 :
    {
	RandomHash_RIPEMD256(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_RIPEMD320 :
    {
	RandomHash_RIPEMD320(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_Blake2b :
    {
	RandomHash_blake2b(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_Blake2s :
    {
	RandomHash_blake2s(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_Tiger2_5_192 :
    {
	RandomHash_Tiger2_5_192(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_Snefru_8_256 :
    {
	RandomHash_Snefru_8_256(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_Grindahl512 :
    {
	RandomHash_Grindahl512(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_Haval_5_256 :
    {
	RandomHash_Haval_5_256(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_MD5 :
    {
	RandomHash_MD5(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_RadioGatun32 :
    {
	RandomHash_RadioGatun32(state->m_data[in_round].roundInput, output);
    } break;
    case RandomHashAlgos::RH_Whirlpool :
    {
	RandomHash_WhirlPool(state->m_data[in_round].roundInput, output);
    } break;
    }

    RH_STRIDE_CHECK_INTEGRITY(output);
    RH_STRIDE_CHECK_INTEGRITY(state->m_data[in_round].roundInput);
}

inline void RandomHash_end(RandomHash_State* state, int in_round)
{
    RH_StridePtr output = RH_STRIDEARRAY_GET(state->m_data[in_round].roundOutputs, RH_STRIDEARRAY_GET_SIZE(state->m_data[in_round].roundOutputs) - 1);
  
    RandomHash_Expand(state, output, in_round, 5 - in_round);
  
    RH_STRIDE_ARRAY_UPDATE_MURMUR3(state->m_data[in_round].roundOutputs, RH_STRIDEARRAY_GET_SIZE(state->m_data[in_round].roundOutputs) - 1);
  
    RH_STRIDEARRAY_RESET(state->m_data[in_round].io_results);
    RH_STRIDEARRAY_PUSHBACK_MANY(state->m_data[in_round].io_results, state->m_data[in_round].roundOutputs);
}

inline void RandomHash_FirstCall_push(RandomHash_State* state, int in_round)
{
    state->m_data[5].in_blockHeader = state->m_data[0].roundInput;
    state->m_data[5].io_results = state->m_data[0].parenAndNeighbortOutputs;
    state->m_skipPhase1 = 0;
}

inline void RandomHash_Block0(RandomHash_State* allStates) {
    RandomHash_State* state = allStates;
    RandomHash_FirstCall_push(state, 5);
    RandomHash_Phase_init(state, 5);
    RandomHash_Phase_1_push(state, 5);
    if (!state->m_skipPhase1) {
	RandomHash_Phase_init(state, 4);
	RandomHash_Phase_1_push(state, 4);
	RandomHash_Phase_init(state, 3);
	RandomHash_Phase_1_push(state, 3);
	RandomHash_Phase_init(state, 2);
	RandomHash_Phase_1_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_1_pop(state, 2);
	RandomHash_Phase_2_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_2_pop(state, 2);
	RandomHash(state, 2);
	RandomHash_end(state, 2);
	RandomHash_Phase_1_pop(state, 3);
	RandomHash_Phase_2_push(state, 3);
	RandomHash_Phase_init(state, 2);
	RandomHash_Phase_1_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_1_pop(state, 2);
	RandomHash_Phase_2_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_2_pop(state, 2);
	RandomHash(state, 2);
	RandomHash_end(state, 2);
	RandomHash_Phase_2_pop(state, 3);
	RandomHash(state, 3);
	RandomHash_end(state, 3);
	RandomHash_Phase_1_pop(state, 4);
	RandomHash_Phase_2_push(state, 4);
	RandomHash_Phase_init(state, 3);
	RandomHash_Phase_1_push(state, 3);
	RandomHash_Phase_init(state, 2);
	RandomHash_Phase_1_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_1_pop(state, 2);
	RandomHash_Phase_2_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_2_pop(state, 2);
	RandomHash(state, 2);
	RandomHash_end(state, 2);
	RandomHash_Phase_1_pop(state, 3);
	RandomHash_Phase_2_push(state, 3);
	RandomHash_Phase_init(state, 2);
	RandomHash_Phase_1_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_1_pop(state, 2);
	RandomHash_Phase_2_push(state, 2);
	RandomHash_Phase_init(state, 1);
	RandomHash_start(state, 1);
	RandomHash(state, 1);
	RandomHash_end(state, 1);
	RandomHash_Phase_2_pop(state, 2);
	RandomHash(state, 2);
	RandomHash_end(state, 2);
	RandomHash_Phase_2_pop(state, 3);
	RandomHash(state, 3);
	RandomHash_end(state, 3);
	RandomHash_Phase_2_pop(state, 4);
	RandomHash(state, 4);
	RandomHash_end(state, 4);
    }
    RandomHash_Phase_1_pop(state, 5);
    RandomHash_Phase_2_push(state, 5);
    RandomHash_Phase_init(state, 4);
    RandomHash_Phase_1_push(state, 4);
    RandomHash_Phase_init(state, 3);
    RandomHash_Phase_1_push(state, 3);
    RandomHash_Phase_init(state, 2);
    RandomHash_Phase_1_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_1_pop(state, 2);
    RandomHash_Phase_2_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_2_pop(state, 2);
    RandomHash(state, 2);
    RandomHash_end(state, 2);
    RandomHash_Phase_1_pop(state, 3);
    RandomHash_Phase_2_push(state, 3);
    RandomHash_Phase_init(state, 2);
    RandomHash_Phase_1_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_1_pop(state, 2);
    RandomHash_Phase_2_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_2_pop(state, 2);
    RandomHash(state, 2);
    RandomHash_end(state, 2);
    RandomHash_Phase_2_pop(state, 3);
    RandomHash(state, 3);
    RandomHash_end(state, 3);
    RandomHash_Phase_1_pop(state, 4);
    RandomHash_Phase_2_push(state, 4);
    RandomHash_Phase_init(state, 3);
    RandomHash_Phase_1_push(state, 3);
    RandomHash_Phase_init(state, 2);
    RandomHash_Phase_1_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_1_pop(state, 2);
    RandomHash_Phase_2_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_2_pop(state, 2);
    RandomHash(state, 2);
    RandomHash_end(state, 2);
    RandomHash_Phase_1_pop(state, 3);
    RandomHash_Phase_2_push(state, 3);
    RandomHash_Phase_init(state, 2);
    RandomHash_Phase_1_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_1_pop(state, 2);
    RandomHash_Phase_2_push(state, 2);
    RandomHash_Phase_init(state, 1);
    RandomHash_start(state, 1);
    RandomHash(state, 1);
    RandomHash_end(state, 1);
    RandomHash_Phase_2_pop(state, 2);
    RandomHash(state, 2);
    RandomHash_end(state, 2);
    RandomHash_Phase_2_pop(state, 3);
    RandomHash(state, 3);
    RandomHash_end(state, 3);
    RandomHash_Phase_2_pop(state, 4);
    RandomHash(state, 4);
    RandomHash_end(state, 4);
    RandomHash_Phase_2_pop(state, 5);
    RandomHash(state, 5);
    RandomHash_end(state, 5);
}

inline void RandomHash_Finalize(RandomHash_State* allStates, U8* output)
{
    RandomHash_State* state = allStates;

    RH_STRIDE_CHECK_INTEGRITY(RH_STRIDEARRAY_GET(state->m_data[5].roundOutputs, 30));

    RandomHash_Compress(state, state->m_data[5].roundOutputs, state->m_data[5].roundInput);

    RH_STRIDE_RESET(state->m_workBytes)
	RandomHash_SHA2_256(state->m_data[5].roundInput, state->m_workBytes);

    memcpy(output, RH_STRIDE_GET_DATA(state->m_workBytes), 32);
}

void RandomHash_Free(void* ptr)
{
    if (ptr)
        RH_SysFree(ptr);
}

void RandomHash_Alloc(void** out_ptr, size_t size)
{
    *out_ptr = RH_SysAlloc(size);
    {
	if (!(*out_ptr)) {
	    exit(-(945));
	}
    };
}


void RandomHash_Simple(U8* input, U8* out_hash)
{
    RandomHash_State state;
    RandomHash_Create(&state);
    RandomHash_RoundDataInit(&state.m_data[0], 0);
    RandomHash_RoundDataInit(&state.m_data[1], 1);
    RandomHash_RoundDataInit(&state.m_data[2], 2);
    RandomHash_RoundDataInit(&state.m_data[3], 3);
    RandomHash_RoundDataInit(&state.m_data[4], 4);
    RandomHash_RoundDataInit(&state.m_data[5], 5);

    state.m_stridesAllocIndex = 0;

    U8* targetInput = state.m_header;
    U8* sourceHeader = &input[0];
    memcpy(targetInput, sourceHeader, PascalHeaderSize);

    (*(U32*)(state.m_data[0].roundInput)) = PascalHeaderSize;
    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(RH_STRIDE_GET_DATA(state.m_data[0].roundInput), &state.m_header[0], PascalHeaderSize);

    RH_STRIDE_INIT(state.m_workBytes);

    RandomHash_Block0(&state);
    RandomHash_Finalize(&state, out_hash);
    RandomHash_Destroy(&state);
}

