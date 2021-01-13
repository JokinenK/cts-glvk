#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSwapchain(
    CtsDevice device,
    const CtsSwapchainCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSwapchain* pSwapchain
);

CtsResult ctsGetSwapchainImages(
    CtsDevice device,
    CtsSwapchain swapchain,
    uint32_t* pSwapchainImageCount,
    CtsImage* pSwapchainImages
);

void ctsDestroySwapchain(
    CtsDevice device,
    CtsSwapchain swapchain,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif