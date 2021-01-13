#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsAllocateMemory(
    CtsDevice device,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
);

CtsResult ctsMapMemory(
    CtsDevice device,
    CtsDeviceMemory memory,
    CtsDeviceSize offset,
    CtsDeviceSize size,
    CtsMemoryMapFlags flags,
    void** pData
);

void ctsUnmapMemory(
    CtsDevice device,
    CtsDeviceMemory memory
);

void ctsFreeMemory(
    CtsDevice device,
    CtsDeviceMemory memory,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif