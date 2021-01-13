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
    CtsInstance instance,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsEnumeratePhysicalDevices(
    CtsInstance instance,
    uint32_t* pPhysicalDeviceCount,
    CtsPhysicalDevice* pPhysicalDevices
);

CtsResult ctsGetPhysicalDeviceQueueFamilyProperties(
    CtsPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    CtsQueueFamilyProperties* pQueueFamilyProperties
);

#ifdef __cplusplus
}
#endif