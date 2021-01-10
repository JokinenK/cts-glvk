#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>
#include <cts/typedefs/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateThreads(
    const CtsThreadCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pThreadCount,
    CtsThread* pThreads
);

bool ctsDestroyThread(
    CtsThread pThread,
    const CtsAllocationCallbacks* pAllocator
);

bool ctsThreadJoinable(
    CtsThread pThread
);

void ctsThreadJoin(
    CtsThread pThread
);

void ctsThreadDetach(
    CtsThread pThread
);

#ifdef __cplusplus
}
#endif