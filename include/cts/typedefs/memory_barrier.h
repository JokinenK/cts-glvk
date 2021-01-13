#pragma once

#include <cts/typedefs/access_flags.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMemoryBarrier {
    CtsStructureType sType;
    const void* pNext;
    CtsAccessFlags srcAccessMask;
    CtsAccessFlags dstAccessMask;
} CtsMemoryBarrier;

#ifdef __cplusplus
}
#endif