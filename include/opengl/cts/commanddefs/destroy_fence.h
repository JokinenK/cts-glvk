#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyFence {
    CtsCmdBase base;
    CtsDevice device;
    CtsFence fence;
    const CtsAllocationCallbacks* allocator;
} CtsDestroyFence;

#ifdef __cplusplus
}
#endif