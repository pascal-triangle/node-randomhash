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

#include "RandomHash_core.h"

using namespace std;

struct RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_RoundData
{
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtr roundInput;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtr otherNonceHeader;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) mersenne_twister_state rndGen;

    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtrArray roundOutputs;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtrArray parenAndNeighbortOutputs;

    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtr in_blockHeader;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtrArray io_results;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtr backup_in_blockHeader;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtrArray backup_io_results;
};

struct RH_ALIGN(RH_IDEAL_ALIGNMENT) RandomHash_State
{
    RH_ALIGN(RH_IDEAL_ALIGNMENT) U8 m_header[PascalHeaderSize];
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_RoundData m_data[RH_N+1];
    RH_ALIGN(RH_IDEAL_ALIGNMENT) U8 m_workBytes[RH_WorkSize];
    RH_ALIGN(RH_IDEAL_ALIGNMENT) mersenne_twister_state m_rndGenCompress;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) mersenne_twister_state m_rndGenExpand;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) U32 m_startNonce;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtr m_stridesInstances;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) U32 m_stridesAllocIndex;

    RH_ALIGN(RH_IDEAL_ALIGNMENT) U8* m_cachedHheader;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) RH_StridePtrArray m_cachedOutputs;
    RH_ALIGN(RH_IDEAL_ALIGNMENT) U32 m_skipPhase1;
};

extern void RandomHash_Create(RandomHash_State* state);
extern void RandomHash_Destroy(RandomHash_State* state);
extern void RandomHash_RoundDataAlloc(RH_RoundData* rd, int round);
extern void RandomHash_RoundDataUnInit(RH_RoundData* rd, int round);
extern void RandomHash_Free(void* ptr);
extern void RandomHash_Alloc(void** out_ptr, size_t size);
extern void RandomHash_Simple(U8* input, U8* out_hash);
  
