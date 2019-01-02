/**
 * rhminer base type functions
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
/// @copyright Polyminer1, QualiaLibre

#ifndef RHMINER_BASE_TYPES_H
#define RHMINER_BASE_TYPES_H

#include <stdint.h>

#define RH_ALIGN(n) __attribute__((aligned(n)))


#define UTIL_LO32(Operand64Bit) ((U32)             ((Operand64Bit) & 0xFFFFFFFF))
#define UTIL_HI32(Operand64Bit) ((U32)             (((U64)(Operand64Bit) >> 32) & 0xFFFFFFFF))
#define RHMINER_JOIN( X, Y )                       RHMINER_DO_JOIN( X, Y )
#define RHMINER_DO_JOIN( X, Y )                    X##Y
#define RHMINER_STRINGIZE(x)                       RHMINER_STRINGIZE2(x)
#define RHMINER_STRINGIZE2(x)                      #x
#define RHMINER_ARRAY_COUNT(arr)                   sizeof(arr) / sizeof(arr[0])
#define RHMINER_KB(X)                              ((X)*1024LLU)
#define RHMINER_MB(X)                              (RHMINER_KB(X)*1024LLU)
#define RHMINER_GB(X)                              (RHMINER_MB(X)*1024LLU)
#define RHMINER_TEST_BIT(VALUE, BIT_MASK)          (((VALUE) & (BIT_MASK)) == (BIT_MASK))

#define RHMINER_PTR_TO_ADDR(ptr)                   ((size_t)ptr)
#define RHMINER_ADDR_TO_PTR(addr)                  ((void*)((size_t)addr)) 
#define RHMINER_ADD_PTR(type, Ptr, Ofs)            ((type) (((size_t)(Ptr)) + (size_t)(Ofs)))
#define RHMINER_SUB_PTR(type, Ptr, Ofs)            ((type) (((size_t)(Ptr)) - (size_t)(Ofs)))
#define RHMINER_ALIGN_PTR(type, Ptr, Ofs)          ((type) RHMINER_FLOOR((size_t)(Ptr), Ofs))
#define RHMINER_DIFF_PTR(First, Second)            ((size_t) (((size_t)(First)) - (size_t)(Second)))
#define RHMINER_ROUND(Val, Align)                  (((Val) + (Align-1)) & ~(Align-1))
#define RHMINER_CEIL(size,to)	                   (((size)+ (to)-1) &~((to)-1))
#define RHMINER_ALIGN(Val, Align)	           (((Val / Align) + 1)*Align)
#define RHMINER_FLOOR(Val, Align)                  ((Val) & ~(Align - 1))

#define _PDBN exit(-(__LINE__));
#define RHMINER_ASSERT(x)			\
    {						\
        if (!(x))				\
        {					\
            _PDBN;				\
        }					\
    }

#include <iostream>

typedef int Device16[16];

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef float F32;
typedef double F64;

const S8 S8_Min = -128;
const S8 S8_Max = 127;
const U8 U8_Min = 0;
const U8 U8_Max = 255;
const S16 S16_Min = -32768;
const S16 S16_Max = 32767;
const U16 U16_Min = 0;
const U16 U16_Max = 65535;
const S32 S32_Min = -2147483647;
const S32 S32_Max = 2147483647;
const U32 U32_Min = 0;
const U32 U32_Max = 4294967295;
const S64 S64_Min = 0x8000000000000000;
const S64 S64_Max = 0x7FFFFFFFFFFFFFFF;
const U64 U64_Min = 0;
const U64 U64_Max = 0xFFFFFFFFFFFFFFFF;
const F32 F32_Min = 1.175494351E-38F;
const F32 F32_Max = 3.402823466E+38F;
const F64 F64_Min = 2.2250738585072014E-308;
const F64 F64_Max = 1.7976931348623158E+308;

#endif //RHMINER_BASE_TYPES_H
