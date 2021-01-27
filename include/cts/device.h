#pragma once

#include <stdint.h>
#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDevice(
    CtsPhysicalDevice physicalDevice,
    const CtsDeviceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDevice* pDevice
);

void ctsGetDeviceQueue(
    CtsDevice device,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex,
    CtsQueue* pQueue
);

void ctsDestroyDevice(
    CtsDevice device,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif