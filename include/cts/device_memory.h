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

CtsResult ctsFlushMappedMemoryRanges(
    CtsDevice device,
    uint32_t memoryRangeCount,
    const CtsMappedMemoryRange* pMemoryRanges
);

void ctsFreeMemory(
    CtsDevice device,
    CtsDeviceMemory memory,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsBindBufferMemory(
    CtsDevice device,
    CtsBuffer buffer, 
    CtsDeviceMemory memory,
    CtsDeviceSize offset
);

CtsResult ctsBindImageMemory(
    CtsDevice device,
    CtsImage image, 
    CtsDeviceMemory memory,
    CtsDeviceSize offset
);

void ctsGetBufferMemoryRequirements(
    CtsDevice device,
    CtsBuffer buffer,
    CtsMemoryRequirements* pMemoryRequirements
);

void ctsGetImageMemoryRequirements(
    CtsDevice device,
    CtsImage image,
    CtsMemoryRequirements* pMemoryRequirements
);

#ifdef __cplusplus
}
#endif