#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateSwapchain {
    CtsCmdBase base;
    VkDevice device;
    const VkSwapchainCreateInfoKHR* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkSwapchainKHR* pSwapchain;
    VkResult* pResult;
} CtsCreateSwapchain;

#ifdef __cplusplus
}
#endif
