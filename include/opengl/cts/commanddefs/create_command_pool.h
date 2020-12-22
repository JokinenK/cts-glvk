#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateCommandPool {
    CtsCmdBase base;
    CtsDevice device;
    const CtsCommandPoolCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsCommandPool* commandPool;
    CtsResult* result;
} CtsCreateCommandPool;

#ifdef __cplusplus
}
#endif