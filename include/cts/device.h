#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDevice(
    CtsPhysicalDevice pPhysicalDevice,
    const CtsDeviceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDevice* pDevice
);

void ctsGetDeviceQueue(
    CtsDevice pDevice,
    uint32_t pQueueFamilyIndex,
    uint32_t pQueueIndex,
    CtsQueue* pQueue
);

void ctsDestroyDevice(
    CtsDevice pDevice,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif