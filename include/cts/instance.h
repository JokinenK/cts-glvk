#pragma once

#include <stdint.h>
#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance* pInstance
);

VkResult VKAPI_CALL ctsDestroyInstance(
    VkInstance instance,
    const VkAllocationCallbacks* pAllocator
);

VkResult VKAPI_CALL ctsEnumerateInstanceVersion(
    uint32_t* pApiVersion
);

VkResult ctsEnumerateInstanceExtensionProperties(
    const char* pLayerName,
    uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties
);

VkResult ctsEnumerateInstanceLayerProperties(
    uint32_t* pPropertyCount,
    VkLayerProperties* pProperties
);

VkResult VKAPI_CALL ctsEnumeratePhysicalDevices(
    VkInstance instance,
    uint32_t* pPhysicalDeviceCount,
    VkPhysicalDevice* pPhysicalDevices
);

#ifdef __cplusplus
}
#endif