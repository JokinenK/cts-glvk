#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyCommandPool {
    CtsCmdBase base;
    CtsDevice device;
    CtsCommandPool commandPool;
    const CtsAllocationCallbacks* allocator;
} CtsDestroyCommandPool;

#ifdef __cplusplus
}
#endif