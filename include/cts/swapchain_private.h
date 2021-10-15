#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/swapchain.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSwapchainEntry {
    VkImage image;
    VkSemaphore semaphore;
};

struct CtsSwapchain {
    struct CtsObjectBase base;
    struct CtsDevice* device;
    struct CtsSurface* surface;
    uint32_t entryCount;
    uint32_t nextEntry;
    struct CtsSwapchainEntry* pEntries;
    VkExtent2D extent;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
};

VkResult ctsCreateSwapchainKHRImpl(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain
);

void ctsDestroySwapchainKHRImpl(
    VkDevice device,
    VkSwapchainKHR swapchain,
    const VkAllocationCallbacks* pAllocator
);

VkResult ctsQueuePresentKHRImpl(
    VkQueue queue,
    const VkPresentInfoKHR* pPresentInfo
);

#ifdef __cplusplus
}
#endif