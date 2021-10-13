#pragma once

#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_win32.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateWin32Surface(
    VkInstance instance,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface
);

void ctsDestroyWin32Surface(
    VkSurfaceKHR surface,
    const VkAllocationCallbacks* pAllocator
);

VkResult ctsGetPhysicalDeviceSurfaceSupport(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR surface,
    VkBool32* pSupported
);

VkResult ctsGetPhysicalDeviceSurfaceCapabilities(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* pSurfaceCapabilities
);

VkResult ctsGetPhysicalDeviceSurfaceFormats(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormatKHR* pSurfaceFormats
);

VkResult ctsGetPhysicalDeviceSurfacePresentModes(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pPresentModeCount,
    VkPresentModeKHR* pPresentModes
);

#ifdef __cplusplus
}
#endif