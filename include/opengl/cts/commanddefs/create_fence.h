#pragma once

#include <cts/allocator.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateFence {
    CtsCmdBase base;
    CtsDevice device;
    const CtsFenceCreateInfo* pCreateInfo;
    const CtsAllocationCallbacks* pAllocator;
    CtsFence* pFence;
    CtsResult* pResult;
} CtsCreateFence;

#ifdef __cplusplus
}
#endif