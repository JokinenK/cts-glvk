#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsGetPhysicalDeviceQueueFamilyProperties(
    CtsPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    CtsQueueFamilyProperties* pQueueFamilyProperties
);

CtsResult ctsEnumerateDeviceExtensionProperties(
    CtsPhysicalDevice physicalDevice,
    const char* pLayerName,
    uint32_t* pPropertyCount,
    CtsExtensionProperties* pProperties
);

void ctsGetPhysicalDeviceMemoryProperties(
    CtsPhysicalDevice physicalDevice,
    CtsPhysicalDeviceMemoryProperties* pMemoryProperties
);

#ifdef __cplusplus
}
#endif