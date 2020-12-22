#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAllocateMemory {
    CtsCmdBase base;
    CtsDevice device;
    const CtsMemoryAllocateInfo* allocateInfo;
    const CtsAllocationCallbacks* allocator;
    CtsDeviceMemory* memory;
    CtsResult* result;
} CtsAllocateMemory;

#ifdef __cplusplus
}
#endif