#pragma once

#include <glad/glad.h>
#include <cts/typedefs/device_memory.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDeviceMemoryImpl {
    
    GLenum handle;
    CtsDeviceSize size;
    uint32_t heapIndex;
    CtsMemoryPropertyFlags propertyFlags;
    const CtsAllocationCallbacks* pAllocator;

    CtsImage images;
    void* pMappedMemory;
};

CtsResult ctsAllocateMemoryImpl(
    CtsDevice device,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
);

CtsResult ctsMapMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory,
    CtsDeviceSize offset,
    CtsDeviceSize size,
    CtsMemoryMapFlags flags,
    void** pData
);

void ctsUnmapMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory
);

CtsResult ctsFlushMappedMemoryRangesImpl(
    CtsDevice device,
    uint32_t memoryRangeCount,
    const CtsMappedMemoryRange* pMemoryRanges
);

void ctsFreeMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif