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
    CtsDevice pDevice,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
);

CtsResult ctsMapMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset,
    CtsDeviceSize pSize,
    CtsMemoryMapFlags pFlags,
    void** pData
);

void ctsUnmapMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory
);

void ctsFreeMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif