#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateFence {
    CtsCmdBase base;
    CtsDevice device;
    const CtsFenceCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsFence* fence;
    CtsResult* result;
} CtsCreateFence;

#ifdef __cplusplus
}
#endif