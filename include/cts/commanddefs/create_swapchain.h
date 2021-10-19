#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/swapchain_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateSwapchainKHR {
    CtsCmdBase base;
    VkDevice device;
    const VkSwapchainCreateInfoKHR* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkSwapchainKHR* pSwapchain;
    VkResult* pResult;
} CtsCreateSwapchainKHR;

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsCreateSwapchainKHR,
    ctsCreateSwapchainKHRImpl,
    device,
    pCreateInfo,
    pAllocator,
    pSwapchain
);

CTS_DEFINE_COMMAND_METADATA(CtsCreateSwapchainKHR);

#ifdef __cplusplus
}
#endif
