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

void ctsGetPhysicalDeviceProperties(
    CtsPhysicalDevice physicalDevice,
    CtsPhysicalDeviceProperties* pProperties
);

void ctsGetPhysicalDeviceMemoryProperties(
    CtsPhysicalDevice physicalDevice,
    CtsPhysicalDeviceMemoryProperties* pMemoryProperties
);

void ctsGetPhysicalDeviceFormatProperties(
    CtsPhysicalDevice physicalDevice,
    CtsFormat format,
    CtsFormatProperties* pFormatProperties
);

#ifdef __cplusplus
}
#endif