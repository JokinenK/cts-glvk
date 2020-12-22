#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMutex* CtsMutex;
typedef struct CtsMutexCreateInfo {
    char unused;
} CtsMutexCreateInfo;

bool ctsCreateMutexes(
    const CtsMutexCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pMutexCount,
    CtsMutex* pMutexes
);

bool ctsDestroyMutex(
    CtsMutex pMutex,
    const CtsAllocationCallbacks* pAllocator
);

void ctsMutexLock(
    CtsMutex pMutex
);

bool ctsMutexTryLock(
    CtsMutex pMutex
);

void ctsMutexUnlock(
    CtsMutex pMutex
);

#ifdef __cplusplus
}
#endif