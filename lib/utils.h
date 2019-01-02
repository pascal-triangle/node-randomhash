/**
 * Various utility functions
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

#pragma once

template<typename _Type>
_Type RH_Min(_Type lsh, _Type rsh) 
{
    return lsh < rsh ? lsh : rsh;
}

template<typename _Type>
_Type RH_Max(_Type lsh, _Type rsh) 
{
    return lsh > rsh ? lsh : rsh;
}

inline uint32_t RH_swap_u32(uint32_t val) 
{
    val = ((val << 8) & 0xFF00FF00u) | ((val >> 8) & 0xFF00FFu); 
    return (val << 16) | ((val >> 16) & 0xFFFFu);
}

inline uint64_t RH_swap_u64(uint64_t a)
{
    return  ((a & 0x00000000000000FFULL) << 56) | 
	    ((a & 0x000000000000FF00ULL) << 40) | 
	    ((a & 0x0000000000FF0000ULL) << 24) | 
	    ((a & 0x00000000FF000000ULL) <<  8) | 
	    ((a & 0x000000FF00000000ULL) >>  8) | 
	    ((a & 0x0000FF0000000000ULL) >> 24) | 
	    ((a & 0x00FF000000000000ULL) >> 40) | 
	    ((a & 0xFF00000000000000ULL) >> 56);
}

///////////////////////////////////////////////////////
// memory
//

extern void* RH_SysAlloc(size_t s);
extern void RH_SysFree(void* ptr);

