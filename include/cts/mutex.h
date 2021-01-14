#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateMutex(
    const CtsMutexCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsMutex* pMutex
);

void ctsDestroyMutex(
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