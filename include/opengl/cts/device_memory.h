#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsAllocateMemory(
    CtsDevice pDevice,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
);

CtsResult ctsMapMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset,
    CtsDeviceSize pSize,
    CtsMemoryMapFlags pFlags,
    void** pData
);

void ctsUnmapMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory
);

void ctsFreeMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif