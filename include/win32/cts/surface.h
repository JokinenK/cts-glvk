#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <Windows.h>
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_win32.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateWin32SurfaceKHR(
    VkInstance instance,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface
);

void VKAPI_CALL ctsDestroySurfaceKHR(
    VkInstance instance,
    VkSurfaceKHR surface,
    const VkAllocationCallbacks* pAllocator
);

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR surface,
    VkBool32* pSupported
);

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* pSurfaceCapabilities
);

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormatKHR* pSurfaceFormats
);

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pPresentModeCount,
    VkPresentModeKHR* pPresentModes
);

VkBool32 ctsGetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex
);

#ifdef __cplusplus
}
#endif