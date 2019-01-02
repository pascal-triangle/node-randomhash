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

#include "basetypes.h"
#include <immintrin.h>

const static U32 PascalHeaderSize = 200;

#define RH_N                    5
#define RH_STRIDE_SIZE_FACTOR   32

// M*=5
#define RH_StrideArrayCount     31
#define RH_StrideSize           208896

#define RH_CheckerSize          (sizeof(U64))
#define RH_WorkSize             RH_StrideSize 
#define RH_IDEAL_ALIGNMENT      64

/// CLEAN BELOW

#define RH_ENABLE_OPTIM_STRIDE_ARRAY_MURMUR3
//#define RH_ENABLE_OPTIM_EXPAND_ACCUM8

#define RH_TOTAL_STRIDES_INSTANCES ((RH_StrideArrayCount+1)*2)


#define RH_GET_MEMBER_POS(STRUCT, MEMBER)  (size_t)(void*)&((STRUCT*)0)->MEMBER

#define copy4(dst, src) {(dst)[0] = (src)[0];	\
	(dst)[1] = (src)[1];			\
	(dst)[2] = (src)[2];			\
	(dst)[3] = (src)[3];}
                               
#define copy4_op(dst, src, op) {(dst)[0] = op((src)[0]);	\
	(dst)[1] = op((src)[1]);				\
	(dst)[2] = op((src)[2]);				\
	(dst)[3] = op((src)[3]);}

#define copy8(dst, src) {(dst)[0] = (src)[0];	\
	(dst)[1] = (src)[1];			\
	(dst)[2] = (src)[2];			\
	(dst)[3] = (src)[3];			\
	(dst)[4] = (src)[4];			\
	(dst)[5] = (src)[5];			\
	(dst)[6] = (src)[6];			\
	(dst)[7] = (src)[7];}

#define copy8_op(dst, src, op) {(dst)[0] = op((src)[0]);	\
	(dst)[1] = op((src)[1]);				\
	(dst)[2] = op((src)[2]);				\
	(dst)[3] = op((src)[3]);				\
	(dst)[4] = op((src)[4]);				\
	(dst)[5] = op((src)[5]);				\
	(dst)[6] = op((src)[6]);				\
	(dst)[7] = op((src)[7]);}

#define copy6_op(dst, src, op) {(dst)[0] = op((src)[0]);	\
	(dst)[1] = op((src)[1]);				\
	(dst)[2] = op((src)[2]);				\
	(dst)[3] = op((src)[3]);				\
	(dst)[4] = op((src)[4]);				\
	(dst)[5] = op((src)[5]);}				\


template<unsigned i>
inline U32 _mm_extract_epi32_( __m128i V)
{
    V = _mm_shuffle_epi32(V, _MM_SHUFFLE(i, i, i, i));
    return (U32)_mm_cvtsi128_si32(V);
}

#define _mm_extract_epi32_M(chunk128, i) (_mm_extract_epi32_<i>(chunk128))

#define RH_MM_LOAD128           _mm_loadu_si128
#define RH_MM_STORE128          _mm_storeu_si128
#define TH_MM_STREAM_STORE128   _mm_storeu_si128
#define RH_MM_BARRIER           void

#ifndef RotateLeft8
#define _rotr8(x,n)	(((x) >> (n)) | ((x) << (8 - (n))))
#define _rotl8(x,n)	(((x) << (n)) | ((x) >> (8 - (n))))
#endif

#ifndef RotateLeft32
#define _rotr(x,n)	(((x) >> (n)) | ((x) << (32 - (n))))
#define _rotl(x,n)	(((x) << (n)) | ((x) >> (32 - (n))))
#define _rotr64(x,n)	(((x) >> (n)) | ((x) << (64 - (n))))
#define _rotl64(x,n)	(((x) << (n)) | ((x) >> (64 - (n))))
#endif
    
#define ROTR32(x,y) _rotr((x),(y))
#define ROTL8(x,y)  _rotl8((x),(U8)((y) % 8))
#define ROTR8(x,y)  _rotr8((x),(U8)((y) % 8))
#define ROTL32(x,y)	_rotl((x),(y))
#define ROTL64(x,y)	_rotl64((x),(y))
#define ROTR64(x,y)	_rotr64((x),(y))
#define ROTL_epi32(m_tmp, m, count) {			\
	m_tmp = m;					\
	m = _mm_slli_epi32(m,count);			\
	m_tmp = _mm_srli_epi32(m_tmp,(32-count));	\
	m = _mm_or_si128(m,m_tmp);}


//--------------------------------------------------------------
#define RH_memzero_8(ptr, size)				\
    {							\
	RH_ASSERT((size_t(ptr) % 8) == 0);		\
	RH_ASSERT((size) == 8);				\
	U64* buf = static_cast<U64*>((void*)(ptr));	\
	*buf = 0;					\
    }


#define RH_memzero_16(ptr, size)		\
    {						\
	RH_ASSERT((size_t(ptr) % 32) == 0);	\
	RH_ASSERT((size) == 16);		\
	__m128i xmm = _mm_set1_epi8((char)0);	\
	RH_MM_STORE128((__m128i *)ptr, xmm);	\
    }

#define RH_memzero_32(ptr, size)			\
    {							\
	RH_ASSERT((size_t(ptr) % 32) == 0);		\
	RH_ASSERT((size) == 32);			\
	__m128i xmm = _mm_set1_epi8((char)0);		\
	RH_MM_STORE128(((__m128i *)ptr)+0, xmm);	\
	RH_MM_STORE128(((__m128i *)ptr)+1, xmm);	\
    }

#define RH_memzero_64(ptr, size)			\
    {							\
	RH_ASSERT((size_t(ptr) % 32) == 0);		\
	RH_ASSERT(size == 64);				\
	__m128i xmm = _mm_set1_epi8((char)0);		\
	RH_MM_STORE128(((__m128i *)ptr)+0, xmm);	\
	RH_MM_STORE128(((__m128i *)ptr)+1, xmm);	\
	RH_MM_STORE128(((__m128i *)ptr)+2, xmm);	\
	RH_MM_STORE128(((__m128i *)ptr)+3, xmm);	\
    }
    

#define RH_memzero_of16(ptr, s)			\
    {						\
	RH_ASSERT((size_t(ptr) % 32) == 0);	\
	RH_ASSERT((s % 16) == 0);		\
	__m128i xmm = _mm_set1_epi8((char)0);	\
	__m128i * buf = (__m128i *)ptr;		\
	size_t size = s / 16;			\
	while (size)				\
	{					\
	    RH_MM_STORE128(buf, xmm);		\
	    buf++;				\
	    size--;				\
	}					\
    }


#define RH_memzero_of8(ptr, s)			\
    {						\
	RH_ASSERT((size_t(ptr) % 32) == 0);	\
	RH_ASSERT((s % 8) == 0);		\
	U64* buf = (U64*)ptr;			\
	size_t size = s / 8;			\
	while (size)				\
	{					\
	    *buf = 0;				\
	    buf++;				\
	    size--;				\
	}					\
    }


//--------------------------------------------------------------


#define RHMINER_T64(x)    ((x) & uint64_t(0xFFFFFFFFFFFFFFFF))


inline
void ReadUInt64AsBytesLE(const uint64_t a_in, uint8_t* a_out)
{
    a_out[0] = (uint8_t)a_in;
    a_out[0 + 1] = (uint8_t)(a_in >> 8);
    a_out[0 + 2] = (uint8_t)(a_in >> 16);
    a_out[0 + 3] = (uint8_t)(a_in >> 24);
    a_out[0 + 4] = (uint8_t)(a_in >> 32);
    a_out[0 + 5] = (uint8_t)(a_in >> 40);
    a_out[0 + 6] = (uint8_t)(a_in >> 48);
    a_out[0 + 7] = (uint8_t)(a_in >> 56);
}


inline
uint64_t ReverseBytesUInt64(const uint64_t value)
{
    return (value & uint64_t(0x00000000000000FF)) << 56 |
	(value & uint64_t(0x000000000000FF00)) << 40 |
	(value & uint64_t(0x0000000000FF0000)) << 24 |
	(value & uint64_t(0x00000000FF000000)) << 8 |
	(value & uint64_t(0x000000FF00000000)) >> 8 |
	(value & uint64_t(0x0000FF0000000000)) >> 24 |
	(value & uint64_t(0x00FF000000000000)) >> 40 |
	(value & uint64_t(0xFF00000000000000)) >> 56;
}


inline
uint32_t ReverseBytesUInt32(const uint32_t value)
{
    return (value & uint32_t(0x000000FF)) << 24 |
	(value & uint32_t(0x0000FF00)) << 8 |
	(value & uint32_t(0x00FF0000)) >> 8 |
	(value & uint32_t(0xFF000000)) >> 24;
}


#define RandomHash_MD_BASE_MAIN_LOOP(ALGO_BLOCK_SIZE, ALGO_FUNC, BE_LE_CONVERT)      { \
	int32_t len = (int32_t)RH_STRIDE_GET_SIZE(roundInput);		\
	uint32_t blockCount = len / ALGO_BLOCK_SIZE;			\
	uint32_t *dataPtr = (uint32_t *)RH_STRIDE_GET_DATA(roundInput);	\
	uint64_t bits = len * 8;					\
	while(blockCount > 0)						\
	{								\
	    ALGO_FUNC(dataPtr, state);					\
	    len -= ALGO_BLOCK_SIZE;					\
	    dataPtr += ALGO_BLOCK_SIZE / 4;				\
	    blockCount--;						\
	}								\
	{								\
	    int32_t padindex;						\
	    RH_ALIGN(64) uint8_t pad[80];				\
									\
	    if (len < 56)						\
		padindex = 56 - len;					\
	    else							\
		padindex = 120 - len;					\
									\
	    RH_memzero_of16(pad, sizeof(pad));				\
	    pad[0] = 0x80;						\
	    bits = BE_LE_CONVERT(bits);					\
	    ReadUInt64AsBytesLE(bits, pad+padindex);			\
									\
	    padindex = padindex + 8;					\
	    memcpy(((uint8_t*)dataPtr) + len, pad, padindex);		\
	    RH_ASSERT(padindex <= 72);					\
	    RH_ASSERT(((padindex + len) % ALGO_BLOCK_SIZE)==0);		\
									\
	    ALGO_FUNC(dataPtr, state);					\
	    padindex -= ALGO_BLOCK_SIZE;				\
	    if (padindex > 0)						\
		ALGO_FUNC(dataPtr+(ALGO_BLOCK_SIZE/4), state);		\
	    RH_ASSERT(padindex > -ALGO_BLOCK_SIZE);			\
	} }

//---------------------------------------------------------------------------------

#define RH_ASSERT(...) 
