#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSwapchain(
    CtsDevice pDevice,
    const CtsSwapchainCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSwapchain* pSwapchain
);

CtsResult ctsGetSwapchainImages(
    CtsDevice pDevice,
    CtsSwapchain pSwapchain,
    uint32_t* pSwapchainImageCount,
    CtsImage* pSwapchainImages
);

void ctsDestroySwapchain(
    CtsDevice pDevice,
    CtsSwapchain pSwapchain,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif