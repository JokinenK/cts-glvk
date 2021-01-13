#pragma once

#include <glad/glad.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDeviceMemory {
    GLenum handle;
    CtsDeviceSize size;
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

void ctsFreeMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif