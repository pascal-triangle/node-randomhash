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

#include <memory.h>
#include <stdint.h>
#include "basetypes.h"
#include "utils.h"


///////////////////////////////////////////////////////
// memory
//

void* RH_SysAlloc(size_t s)
{
    //return _mm_malloc( s, 4096 );
    const int ALIGNVAL = 4096;
    void* ptr = malloc(s + ALIGNVAL + sizeof(size_t));
    size_t ptrAli = ((size_t)ptr) + ALIGNVAL-(((size_t)ptr) % ALIGNVAL);
    U8* ptrAliBack = ((U8*)ptrAli) - sizeof(size_t);
    *(size_t*)ptrAliBack = (size_t)ptr;
    return (void*)ptrAli;
}

void RH_SysFree(void* ptr)
{
    //_mm_free(ptr);
    size_t* ptrAliBack = (size_t*)(((U8*)ptr) - sizeof(size_t));
    free((void*)*ptrAliBack);
}

