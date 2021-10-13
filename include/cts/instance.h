#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance* pInstance
);

VkResult ctsDestroyInstance(
    VkInstance instance,
    const VkAllocationCallbacks* pAllocator
);

VkResult ctsEnumeratePhysicalDevices(
    VkInstance instance,
    uint32_t* pPhysicalDeviceCount,
    VkPhysicalDevice* pPhysicalDevices
);

#ifdef __cplusplus
}
#endif