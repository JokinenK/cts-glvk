#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/swapchain_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySwapchainKHR {
    CtsCmdBase base;
    VkDevice device;
    VkSwapchainKHR swapchain;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroySwapchainKHR;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsDestroySwapchainKHR,
    ctsDestroySwapchainKHRImpl,
    device,
    swapchain,
    pAllocator
);

#ifdef __cplusplus
}
#endif