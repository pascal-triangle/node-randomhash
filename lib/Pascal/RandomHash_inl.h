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

#pragma once
#include "RandomHash_def.h"

// Hard coded this value to false
bool g_isSSE41Supported = false;

inline void memcpy_uncached_load_sse41_SPILL(void *dest, const void *src, size_t n)
{
    char *d = (char*)dest;
    uintptr_t d_int = (uintptr_t)d;
    const char *s = (const char *)src;
    uintptr_t s_int = (uintptr_t)s;


    if (s_int & 0xf)
    {
	size_t nh = RH_Min(0x10 - (s_int & 0x0f), n);

	*(U64*)d = *(U64*)s;
	if (nh > 8)
        {
	    nh -= 8;
	    d += 8;
	    d_int += 8;
	    s += 8;
	    s_int += 8;
	    n -= nh;
	    *(U64*)d = *(U64*)s;
        }
	d += nh;
	d_int += nh;
	s += nh;
	s_int += nh;
	n -= nh;
    }

    if (d_int & 0xf)
    {
	__m128i r0,r1,r2,r3,r4,r5,r6,r7;
	while (n >= 8*sizeof(__m128i)) {
	    r0 = RH_MM_LOAD128 ((__m128i *)(s));
	    r1 = RH_MM_LOAD128 ((__m128i *)(s+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(s+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(s+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(s+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(s+5*sizeof(__m128i)));
	    r6 = RH_MM_LOAD128 ((__m128i *)(s+6*sizeof(__m128i)));
	    r7 = RH_MM_LOAD128 ((__m128i *)(s+7*sizeof(__m128i)));
	    RH_MM_STORE128((__m128i *)(d), r0);
	    RH_MM_STORE128((__m128i *)(d+1*sizeof(__m128i)), r1);
	    RH_MM_STORE128((__m128i *)(d+2*sizeof(__m128i)), r2);
	    RH_MM_STORE128((__m128i *)(d+3*sizeof(__m128i)), r3);
	    RH_MM_STORE128((__m128i *)(d+4*sizeof(__m128i)), r4);
	    RH_MM_STORE128((__m128i *)(d+5*sizeof(__m128i)), r5);
	    RH_MM_STORE128((__m128i *)(d+6*sizeof(__m128i)), r6);
	    RH_MM_STORE128((__m128i *)(d+7*sizeof(__m128i)), r7);
	    s += 8*sizeof(__m128i);
	    d += 8*sizeof(__m128i);
	    n -= 8*sizeof(__m128i);
	}
	while (n >= sizeof(__m128i))
        {
	    r0 = RH_MM_LOAD128 ((__m128i *)(s));
	    RH_MM_STORE128((__m128i *)(d), r0);
	    s += sizeof(__m128i);
	    d += sizeof(__m128i);
	    n -= sizeof(__m128i);
        }
    }
    else
    {

	__m128i r0,r1,r2,r3,r4,r5,r6,r7;
	while (n >= 8*sizeof(__m128i)) {
	    r0 = RH_MM_LOAD128 ((__m128i *)(s));
	    r1 = RH_MM_LOAD128 ((__m128i *)(s+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(s+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(s+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(s+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(s+5*sizeof(__m128i)));
	    r6 = RH_MM_LOAD128 ((__m128i *)(s+6*sizeof(__m128i)));
	    r7 = RH_MM_LOAD128 ((__m128i *)(s+7*sizeof(__m128i)));
	    TH_MM_STREAM_STORE128((__m128i *)(d), r0);
	    TH_MM_STREAM_STORE128((__m128i *)(d+1*sizeof(__m128i)), r1);
	    TH_MM_STREAM_STORE128((__m128i *)(d+2*sizeof(__m128i)), r2);
	    TH_MM_STREAM_STORE128((__m128i *)(d+3*sizeof(__m128i)), r3);
	    TH_MM_STREAM_STORE128((__m128i *)(d+4*sizeof(__m128i)), r4);
	    TH_MM_STREAM_STORE128((__m128i *)(d+5*sizeof(__m128i)), r5);
	    TH_MM_STREAM_STORE128((__m128i *)(d+6*sizeof(__m128i)), r6);
	    TH_MM_STREAM_STORE128((__m128i *)(d+7*sizeof(__m128i)), r7);
	    s += 8*sizeof(__m128i);
	    d += 8*sizeof(__m128i);
	    n -= 8*sizeof(__m128i);
	}
	while (n >= sizeof(__m128i)) {
	    r0 = RH_MM_LOAD128 ((__m128i *)(s));
	    TH_MM_STREAM_STORE128((__m128i *)(d), r0);
	    s += sizeof(__m128i);
	    d += sizeof(__m128i);
	    n -= sizeof(__m128i);
	}
    }

    if (n)
    {
	RH_ASSERT(n <= 16);
	*(U64*)d = *(U64*)s;
	if (n > 8)
        {
	    d += 8;
	    s += 8;
	    *(U64*)d = *(U64*)s;
        }
    }

    _mm_sfence();
}


inline void RH_INPLACE_MEMCPY_128_A(U8* pDst, U8* pSrc, U32 byteCount, MurmurHash3_x86_32_State* accum)
{
    RH_ASSERT(( (size_t)pDst % 8) == 0);
    RH_ASSERT(( (size_t)pSrc % 8) == 0);

    S32 n = (byteCount / sizeof(__m128i)) * sizeof(__m128i);
    U32 m = byteCount % sizeof(__m128i);

    __m128i r0;
    while (n > 0)
    {
	r0 = RH_MM_LOAD128((__m128i *)(pSrc ));
	MurmurHash3_x86_32_Update_16(r0, 16, accum);

	RH_MM_STORE128((__m128i *)(pDst ), r0);
	pSrc += sizeof(__m128i);
	pDst += sizeof(__m128i);
	n -= sizeof(__m128i);
    }
    if (m)
    {
	r0 = RH_MM_LOAD128((__m128i *)(pSrc));
	RH_MM_STORE128((__m128i *)(pDst ), r0);
	MurmurHash3_x86_32_Update_16(r0, m, accum);

    }
    RH_MM_BARRIER();

}


inline void RH_STRIDE_MEMCPY_ALIGNED_SIZE128(U8 *pDst, U8 *pSrc, size_t byteCount)
{
    RH_ASSERT(( (size_t)pDst % 8) == 0);
    RH_ASSERT(( (size_t)pSrc % 8) == 0);
    RH_ASSERT(( (size_t)pDst % 32) == 0);
    RH_ASSERT(( (size_t)pSrc % 32) == 0);

    {S32 n = RHMINER_CEIL(byteCount, sizeof(__m128i));
	__m128i r0;
	while (n >= sizeof(__m128i)) { r0 = RH_MM_LOAD128 ((__m128i *)(pSrc));
	    RH_MM_STORE128((__m128i *)(pDst), r0);
	    pSrc += sizeof(__m128i);
	    pDst += sizeof(__m128i);
	    n -= sizeof(__m128i);
	} RH_MM_BARRIER();
    };

}


inline void RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(U8 *pDst, U8 *pSrc, size_t byteCount)
{
    RH_ASSERT(byteCount < RH_StrideSize - 256);

    size_t s = byteCount;
    memcpy_uncached_load_sse41_SPILL(pDst, pSrc, s);
    return;

    if (((size_t)pDst)%8 == 0 &&
	((size_t)pSrc)%8 == 0)
    {
	U8* end = pDst + byteCount;
	while(pDst < end)
        {
	    *(U64*)pDst = *(U64*)pSrc;
	    pDst += 8;
	    pSrc += 8;
        }
    }
    else
    {
	memcpy((void*)(pDst), (void*)(pSrc), byteCount);
    }
}


void RH_STRIDE_ARRAY_UPDATE_MURMUR3(U8* strideArray, U32 elementIdx)
{    
    RH_StridePtr lstride = RH_STRIDEARRAY_GET(strideArray, elementIdx);
    U32 size = RH_STRIDE_GET_SIZE(lstride);
    lstride = RH_STRIDE_GET_DATA(lstride);

    RH_MUR3_BACKUP_STATE(RH_StrideArrayStruct_GetAccum(strideArray));
    back_i = 0;   
    RH_ASSERT(back_idx != 0xDEADBEEF)                        
	RH_ASSERT(back_idx == 0);

    register uint32_t h1 = back_h1;
    RH_ASSERT(size >= sizeof(U64));
    RH_ASSERT(( (size_t)strideArray % 8) == 0);
    S32 n = (size / sizeof(U64)) * sizeof(U64);
    U32 m = size % sizeof(U64);    
    RH_StridePtr lstride_end = lstride + n;
    U64 r0;
    back_totalLen += n;
    while (lstride != lstride_end)
    {
        r0 = *(U64*)(lstride);
        MURMUR3_BODY((U32)(r0));
        MURMUR3_BODY((U32)(r0 >> 32));                                                           
        lstride += sizeof(U64);        
    }
    back_h1 = h1;
    if (m)
    {
        U64 r0 = *((U64 *)(lstride));
        INPLACE_M_MurmurHash3_x86_32_Update_8(r0, m); 
    }

    RH_MUR3_RESTORE_STATE(RH_StrideArrayStruct_GetAccum(strideArray));
}


void RH_STRIDEARRAY_PUSHBACK_MANY(U8* strideArrayVar, U8* strideArrayVarSrc)
{
    RH_ASSERT(strideArrayVar != strideArrayVarSrc);
    RH_ASSERT(RH_STRIDEARRAY_GET_SIZE(strideArrayVar) <= RH_StrideArrayCount);
    if (RH_STRIDEARRAY_GET_SIZE(strideArrayVar) == 0)
    {
        *RH_StrideArrayStruct_GetAccum(strideArrayVar) = *RH_StrideArrayStruct_GetAccum(strideArrayVarSrc);
        RH_STRIDEARRAY_FOR_EACH_BEGIN(strideArrayVarSrc)
        {
            RH_STRIDEARRAY_PUSHBACK_NO_ACCUM(strideArrayVar, strideItrator);
        }
        RH_STRIDEARRAY_FOR_EACH_END(strideArrayVarSrc)
	    }
    else
    {
        RH_STRIDEARRAY_FOR_EACH_BEGIN(strideArrayVarSrc)
        {
            RH_STRIDEARRAY_PUSHBACK(strideArrayVar, strideItrator);
            RH_STRIDE_ARRAY_UPDATE_MURMUR3((strideArrayVar), RH_STRIDEARRAY_GET_SIZE(strideArrayVar)-1);
        }
        RH_STRIDEARRAY_FOR_EACH_END(strideArrayVarSrc);
    }
}



inline void RH_STRIDEARRAY_CLONE(U8* strideArrayVar, U8* strideArrayVarSrc, RandomHash_State* state)
{
    RH_StridePtrArray target = strideArrayVar;
    RH_STRIDEARRAY_RESET(target);
    RH_STRIDEARRAY_FOR_EACH_BEGIN(strideArrayVarSrc)
    {
	U32 _as = RH_STRIDEARRAY_GET_SIZE(target)++;
	RH_STRIDEARRAY_GET(target, _as) = RH_StrideArrayAllocOutput(state);
	RH_StridePtr elem = RH_STRIDEARRAY_GET(target, _as);
	RH_STRIDE_COPY(elem, (strideItrator));
    }
    RH_STRIDEARRAY_FOR_EACH_END(strideArrayVarSrc);

}

inline void Transfo0(U8* nextChunk, U32 size, U8* workBytes)
{
    U32 rndState = MurmurHash3_x86_32_Fast((const void *)nextChunk,size, 0);
    if (!rndState)
	rndState = 1;

    if (size <= 128 && (size_t(nextChunk)&0x0f)==0 && g_isSSE41Supported)
    {

	__m128i r0,r1,r2,r3,r4,r5,r6,r7;
	switch(size/16)
        {
	case 8:
	case 7: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(nextChunk+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(nextChunk+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(nextChunk+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(nextChunk+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(nextChunk+5*sizeof(__m128i)));
	    r6 = RH_MM_LOAD128 ((__m128i *)(nextChunk+6*sizeof(__m128i)));
	    r7 = RH_MM_LOAD128 ((__m128i *)(nextChunk+7*sizeof(__m128i)));
	    break;
	case 6: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(nextChunk+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(nextChunk+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(nextChunk+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(nextChunk+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(nextChunk+5*sizeof(__m128i)));
	    r6 = RH_MM_LOAD128 ((__m128i *)(nextChunk+6*sizeof(__m128i)));
	    break;
	case 5: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(nextChunk+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(nextChunk+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(nextChunk+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(nextChunk+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(nextChunk+5*sizeof(__m128i)));
	    break;
	case 4: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(nextChunk+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(nextChunk+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(nextChunk+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(nextChunk+4*sizeof(__m128i)));
	    break;
	case 3: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(nextChunk+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(nextChunk+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(nextChunk+3*sizeof(__m128i)));
	    break;
	case 2: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(nextChunk+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(nextChunk+2*sizeof(__m128i)));
	    break;
	case 1: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(nextChunk+1*sizeof(__m128i)));
	    break;
	case 0: r0 = RH_MM_LOAD128 ((__m128i *)(nextChunk+0*sizeof(__m128i)));
	    break;
	default: RHMINER_ASSERT(false);
        }

	U8* head = nextChunk;
	U8* end = head + size;

	while(head < end)
        {
	    uint32_t x = rndState;
	    x ^= x << 13;
	    x ^= x >> 17;
	    x ^= x << 5;
	    rndState = x;
	    U32 d;

	    U8 b;
	    U32 val = x % size;
	    U32 reg = val / 16;
	    U32 n = val % 16;
	    switch(reg)
	    {
	    case 7: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r7, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r7, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r7, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r7, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 6: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r6, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r6, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r6, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r6, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 5: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r5, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r5, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r5, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r5, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 4: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r4, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r4, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r4, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r4, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 3: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r3, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r3, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r3, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r3, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 2: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r2, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r2, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r2, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r2, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 1: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r1, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r1, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r1, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r1, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 0: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r0, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r0, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r0, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r0, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    default: RHMINER_ASSERT(false);

	    }

	    *head = b;
	    head++;
        }
    }
    else

    {
	RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(&workBytes[0], nextChunk, size);
	U8* head = nextChunk;
	U8* end = head + size;
	while(head < end)
        {
	    uint32_t x = rndState;
	    x ^= x << 13;
	    x ^= x >> 17;
	    x ^= x << 5;
	    rndState = x;
	    *head = workBytes[x % size];
	    head++;
        }
    }


}

inline void Transfo0_2(U8* nextChunk, U32 size, U8* source)
{
    U32 rndState = MurmurHash3_x86_32_Fast((const void *)source,size, 0);
    if (!rndState)
	rndState = 1;

    if (size <= 128 && (size_t(source)&0x0f)==0 && g_isSSE41Supported)
    {

	__m128i r0,r1,r2,r3,r4,r5,r6,r7;
	switch(size/16)
        {
	case 8:
	case 7: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(source+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(source+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(source+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(source+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(source+5*sizeof(__m128i)));
	    r6 = RH_MM_LOAD128 ((__m128i *)(source+6*sizeof(__m128i)));
	    r7 = RH_MM_LOAD128 ((__m128i *)(source+7*sizeof(__m128i)));
	    break;
	case 6: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(source+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(source+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(source+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(source+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(source+5*sizeof(__m128i)));
	    r6 = RH_MM_LOAD128 ((__m128i *)(source+6*sizeof(__m128i)));
	    break;
	case 5: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(source+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(source+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(source+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(source+4*sizeof(__m128i)));
	    r5 = RH_MM_LOAD128 ((__m128i *)(source+5*sizeof(__m128i)));
	    break;
	case 4: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(source+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(source+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(source+3*sizeof(__m128i)));
	    r4 = RH_MM_LOAD128 ((__m128i *)(source+4*sizeof(__m128i)));
	    break;
	case 3: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(source+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(source+2*sizeof(__m128i)));
	    r3 = RH_MM_LOAD128 ((__m128i *)(source+3*sizeof(__m128i)));
	    break;
	case 2: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(source+1*sizeof(__m128i)));
	    r2 = RH_MM_LOAD128 ((__m128i *)(source+2*sizeof(__m128i)));
	    break;
	case 1: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    r1 = RH_MM_LOAD128 ((__m128i *)(source+1*sizeof(__m128i)));
	    break;
	case 0: r0 = RH_MM_LOAD128 ((__m128i *)(source+0*sizeof(__m128i)));
	    break;
	default: RHMINER_ASSERT(false);
        }

	U8* head = nextChunk;
	U8* end = head + size;

	while(head < end)
        {
	    uint32_t x = rndState;
	    x ^= x << 13;
	    x ^= x >> 17;
	    x ^= x << 5;
	    rndState = x;
	    U32 d;

	    U8 b;
	    U32 val = x % size;
	    U32 reg = val / 16;
	    U32 n = val % 16;
	    switch(reg)
	    {
	    case 7: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r7, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r7, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r7, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r7, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 6: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r6, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r6, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r6, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r6, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 5: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r5, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r5, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r5, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r5, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 4: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r4, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r4, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r4, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r4, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 3: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r3, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r3, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r3, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r3, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 2: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r2, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r2, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r2, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r2, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 1: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r1, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r1, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r1, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r1, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    case 0: {
		d = ((n) & 0x7)*8;
		switch((n)>>2) {
		case 0:b = _mm_extract_epi32_M(r0, 0)>>d;
		    break;
		case 1:b = _mm_extract_epi32_M(r0, 1)>>d;
		    break;
		case 2:b = _mm_extract_epi32_M(r0, 2)>>d;
		    break;
		case 3:b = _mm_extract_epi32_M(r0, 3)>>d;
		    break;
		default: RHMINER_ASSERT(false);
		};
	    } break;

	    default: RHMINER_ASSERT(false);

            }

	    *head = b;
	    head++;
        }
    }
    else

    {
	U8* head = nextChunk;
	U8* end = head + size;
	while(head < end)
        {
	    uint32_t x = rndState;
	    x ^= x << 13;
	    x ^= x >> 17;
	    x ^= x << 5;
	    rndState = x;
	    *head = source[x % size];
	    head++;
        }
    }
}

inline void Transfo1(U8* nextChunk, U32 size, U8* workBytes)
{
    U32 halfSize = size >> 1;
    U32 sizeIsOdd = size % 2;

    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(&workBytes[0], nextChunk, halfSize);

    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(nextChunk, nextChunk + halfSize + sizeIsOdd, halfSize);
    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(nextChunk + halfSize + sizeIsOdd, &workBytes[0], halfSize);
    if (sizeIsOdd)
	nextChunk[halfSize] = workBytes[halfSize];

}

inline void Transfo1_2(U8* nextChunk, U32 size, U8* outputPtr)
{
    U32 halfSize = size >> 1;
    U32 sizeIsOdd = size % 2;

    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(nextChunk, outputPtr + halfSize + sizeIsOdd, halfSize);
    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(nextChunk + halfSize + sizeIsOdd, outputPtr, halfSize);
    if (sizeIsOdd)
	nextChunk[halfSize] = outputPtr[halfSize];

}

inline void Transfo2(U8* nextChunk, U32 size, U8* workBytes)
{
    U32 halfSize = size >> 1;

    U8 b;
    U8* head = nextChunk;
    U8* end = head + halfSize;
    U8* tail = &nextChunk[size - 1];
    while(head < end)
    {
	b = *head;
	*head = *tail;
	*tail = b;
	head++;
	tail--;
    }

}

inline void Transfo2_2(U8* nextChunk, U32 size, U8* outputPtr)
{
    U32 halfSize = size >> 1;

    U8* srcHead = outputPtr;
    U8* srcEnd = srcHead + halfSize;
    U8* srcTail = &outputPtr[size - 1];
    U8* tail = &nextChunk[size - 1];
    U8* head = nextChunk;
    while(srcHead < srcEnd)
    {
	*head = *srcTail;
	*tail = *srcHead;
	head++;
	tail--;
	srcHead++;
	srcTail--;
    }
}



inline void Transfo3(U8* nextChunk, U32 size, U8* workBytes)
{
    U32 sizeIsOdd = size % 2;
    U32 halfSize = size >> 1;
    int left = 0;
    int right = (int)halfSize + sizeIsOdd;

    U8* work = workBytes;
    while(left < halfSize)
    {
	*work = nextChunk[left++];
	work++;
	*work = nextChunk[right++];
	work++;
    }
    if (sizeIsOdd)
	*work = nextChunk[halfSize];

    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(nextChunk, workBytes, size);

    RH_ASSERT(size < RH_StrideSize);


}


inline void Transfo3_2(U8* nextChunk, U32 size, U8* outputPtr)
{
    U32 sizeIsOdd = size % 2;
    U32 halfSize = size >> 1;
    int left = 0;
    int right = (int)halfSize + sizeIsOdd;
    U8* work = nextChunk;
    while(left < halfSize)
    {
	*work = outputPtr[left++];
	work++;
	*work = outputPtr[right++];
	work++;
    }
    if (sizeIsOdd)
	*work = outputPtr[halfSize];

    RH_ASSERT(size < RH_StrideSize);
}

inline void Transfo4(U8* nextChunk, U32 size, U8* workBytes)
{
    U32 sizeIsOdd = size % 2;
    U32 halfSize = size >> 1;

    int left = 0;
    int right = halfSize + sizeIsOdd;
    U8* work = workBytes;
    while(left < halfSize)
    {
	*work = nextChunk[right++];
	work++;
	*work = nextChunk[left++];
	work++;
    }
    if (sizeIsOdd)
	*work = nextChunk[halfSize];

    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(nextChunk, workBytes, size);

    RH_ASSERT(size < RH_StrideSize);

}

inline void Transfo4_2(U8* nextChunk, U32 size, U8* outputPtr)
{
    U32 sizeIsOdd = size % 2;
    U32 halfSize = size >> 1;

    int left = 0;
    int right = halfSize + sizeIsOdd;
    U8* work = nextChunk;
    while(left < halfSize)
    {
	*work = outputPtr[right++];
	work++;
	*work = outputPtr[left++];
	work++;
    }
    if (sizeIsOdd)
	*work = outputPtr[halfSize];
}


inline void Transfo5(U8* nextChunk, U32 size, U8* workBytes)
{
    U32 sizeIsOdd = size % 2;
    U32 halfSize = size >> 1;
    U32 i = 0;
    S32 itt = 0;
    S32 ritt = size-1;


    RH_ASSERT(size < RH_StrideSize);

    RH_STRIDE_MEMCPY_UNALIGNED_SIZE8(workBytes, nextChunk, size);

    while(i < halfSize)
    {

	nextChunk[i] = workBytes[itt] ^ workBytes[itt + 1];
	itt += 2;

	nextChunk[i+halfSize + sizeIsOdd] = workBytes[i] ^ workBytes[ritt--];
	i++;
    }

    if (sizeIsOdd)
	nextChunk[halfSize] = workBytes[size-1];
}

inline void Transfo5_2(U8* nextChunk, U32 size, U8* outputPtr)
{
    U32 sizeIsOdd = size % 2;
    U32 halfSize = size >> 1;
    U32 i = 0;
    S32 itt = 0;
    S32 ritt = size-1;

    while(i < halfSize)
    {

	nextChunk[i] = outputPtr[itt] ^ outputPtr[itt + 1];
	itt += 2;

	nextChunk[i+halfSize + sizeIsOdd] = outputPtr[i] ^ outputPtr[ritt];
	i++;
	ritt--;
    }

    if (sizeIsOdd)
	nextChunk[halfSize] = outputPtr[size-1];
}

inline void Transfo6(U8* nextChunk, U32 size)
{
    U32 i = 0;

    U8* end = nextChunk + (size >> 3)*8;
    while(((size_t)nextChunk % 8) != 0 && (i < size))
    {
	*nextChunk = ROTL8(*nextChunk, size-i);
	i++;
	nextChunk++;
    }
    while(nextChunk < end)
    {
	U8 b;
	U64 res = 0;
	U64 buf = *(U64*)nextChunk;
	U32 localSize = size - i;


	b = (U8)(buf);
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b));
	b = (U8)(buf >> (1<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(1 << 3));
	b = (U8)(buf >> (2<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(2 << 3));
	b = (U8)(buf >> (3<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(3 << 3));
	b = (U8)(buf >> (4<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(4 << 3));
	b = (U8)(buf >> (5<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(5 << 3));
	b = (U8)(buf >> (6<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(6 << 3));
	b = (U8)(buf >> (7<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(7 << 3));
	i += 8;
	*(U64*)nextChunk = res;
	nextChunk += 8;
    }
    while(i < size)
    {
	*nextChunk = ROTL8(*nextChunk, size-i);
	i++;
	nextChunk++;
    }

}

inline void Transfo6_2(U8* nextChunk, U32 size, U8* source)
{
    U32 i = 0;

    U8* end = nextChunk + (size >> 3)*8;
    while(((size_t)source % 8) != 0 && (i < size))
    {
	*nextChunk = ROTL8(*source, size-i);
	i++;
	nextChunk++;
	source++;
    }
    while(nextChunk < end)
    {

	U8 b;
	U64 res = 0;
	U64 buf = *(U64*)source;
	U32 localSize = size - i;


	b = (U8)(buf);
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b));
	b = (U8)(buf >> (1<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(1 << 3));
	b = (U8)(buf >> (2<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(2 << 3));
	b = (U8)(buf >> (3<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(3 << 3));
	b = (U8)(buf >> (4<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(4 << 3));
	b = (U8)(buf >> (5<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(5 << 3));
	b = (U8)(buf >> (6<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(6 << 3));
	b = (U8)(buf >> (7<<3));
	b = ROTL8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(7 << 3));

	i += 8;
	*(U64*)nextChunk = res;
	nextChunk += 8;
	source += 8;
    }
    while(i < size)
    {
	*nextChunk = ROTL8(*source, size-i);
	i++;
	nextChunk++;
	source++;
    }

}

inline void Transfo7(U8* nextChunk, U32 size)
{
    U32 i = 0;

    U8* end = nextChunk + (size >> 3)*8;
    while(((size_t)nextChunk % 8) != 0 && (i < size))
    {
	*nextChunk = ROTR8(*nextChunk, size-i);
	i++;
	nextChunk++;
    }
    while(nextChunk < end)
    {
	U8 b;
	U64 res = 0;
	U64 buf = *(U64*)nextChunk;
	U32 localSize = size - i;


	b = (U8)(buf);
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b));
	b = (U8)(buf >> (1<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(1 << 3));
	b = (U8)(buf >> (2<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(2 << 3));
	b = (U8)(buf >> (3<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(3 << 3));
	b = (U8)(buf >> (4<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(4 << 3));
	b = (U8)(buf >> (5<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(5 << 3));
	b = (U8)(buf >> (6<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(6 << 3));
	b = (U8)(buf >> (7<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(7 << 3));
	i += 8;
	*(U64*)nextChunk = res;
	nextChunk += 8;
    }
    while(i < size)
    {
	*nextChunk = ROTR8(*nextChunk, size-i);
	i++;
	nextChunk++;
    }

}

inline void Transfo7_2(U8* nextChunk, U32 size, U8* source)
{
    U32 i = 0;

    U8* end = nextChunk + (size >> 3)*8;
    while(((size_t)source % 8) != 0 && (i < size))
    {
	*nextChunk = ROTR8(*source, size-i);
	i++;
	nextChunk++;
	source++;
    }
    while(nextChunk < end)
    {
	U8 b;
	U64 res = 0;
	U64 buf = *(U64*)source;
	U32 localSize = size - i;


	b = (U8)(buf);
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b));
	b = (U8)(buf >> (1<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(1 << 3));
	b = (U8)(buf >> (2<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(2 << 3));
	b = (U8)(buf >> (3<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(3 << 3));
	b = (U8)(buf >> (4<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(4 << 3));
	b = (U8)(buf >> (5<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(5 << 3));
	b = (U8)(buf >> (6<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(6 << 3));
	b = (U8)(buf >> (7<<3));
	b = ROTR8(b, localSize); localSize--;
	res |= ((U64)(b) << (U64)(7 << 3));
	i += 8;
	*(U64*)nextChunk = res;
	nextChunk += 8;
	source += 8;
    }
    while(i < size)
    {
	*nextChunk = ROTR8(*source, size-i);
	i++;
	nextChunk++;
	source++;
    }

}
