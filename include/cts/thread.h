#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateThread(
    const CtsThreadCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsThread* pThread
);

void ctsDestroyThread(
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