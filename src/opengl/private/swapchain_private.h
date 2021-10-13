#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/swapchain.h"

#ifdef __cplusplus
extern "C" {
#endif

static const VkExtensionProperties swapchainExtensionProperties = {
    .extensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    .specVersion = 1,
};

struct CtsSwapchainEntry {
    VkImage image;
    VkSemaphore semaphore;
};

struct CtsSwapchain {
    struct CtsDevice* device;
    uint32_t entryCount;
    uint32_t nextEntry;
    struct CtsSwapchainEntry* pEntries;
    VkExtent2D extent;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
};

VkResult ctsCreateSwapchainImpl(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain
);

void ctsDestroySwapchainImpl(
    VkDevice device,
    VkSwapchainKHR swapchain,
    const VkAllocationCallbacks* pAllocator
);

VkResult ctsQueuePresentImpl(
    VkQueue queue,
    const VkPresentInfoKHR* pPresentInfo
);

#ifdef __cplusplus
}
#endif