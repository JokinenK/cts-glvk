#pragma once

#include <cts/allocator.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAllocateMemory {
    CtsCmdBase base;
    CtsDevice device;
    const CtsMemoryAllocateInfo* pAllocateInfo;
    const CtsAllocationCallbacks* pAllocator;
    CtsDeviceMemory* pMemory;
    CtsResult* pResult;
} CtsAllocateMemory;

#ifdef __cplusplus
}
#endif