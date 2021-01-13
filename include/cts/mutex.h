#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>
#include <cts/typedefs/mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateMutexes(
    const CtsMutexCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t mutexCount,
    CtsMutex* pMutexes
);

bool ctsDestroyMutex(
    CtsMutex mutex,
    const CtsAllocationCallbacks* pAllocator
);

void ctsMutexLock(
    CtsMutex mutex
);

bool ctsMutexTryLock(
    CtsMutex mutex
);

void ctsMutexUnlock(
    CtsMutex mutex
);

#ifdef __cplusplus
}
#endif