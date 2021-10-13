#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySwapchain {
    CtsCmdBase base;
    VkDevice device;
    VkSwapchainKHR swapchain;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroySwapchain;

#ifdef __cplusplus
}
#endif