#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateInstance(
    const CtsInstanceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsInstance* pInstance
);

CtsResult ctsDestroyInstance(
    CtsInstance pInstance,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsEnumeratePhysicalDevices(
    CtsInstance pInstance,
    uint32_t* pPhysicalDeviceCount,
    CtsPhysicalDevice* pPhysicalDevices
);

CtsResult ctsGetPhysicalDeviceQueueFamilyProperties(
    CtsPhysicalDevice pPhysicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    CtsQueueFamilyProperties* pQueueFamilyProperties
);

#ifdef __cplusplus
}
#endif