#pragma once

#include <stdint.h>
#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CTS_SWAPCHAIN_EXTENSION_NAME "CTS_swapchain"

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

CtsResult ctsAcquireNextImage(
    CtsDevice device,
    CtsSwapchain swapchain,
    uint64_t timeout,
    CtsSemaphore semaphore,
    CtsFence fence,
    uint32_t* pImageIndex
);

CtsResult ctsQueuePresent(
    CtsQueue queue,
    const CtsPresentInfo* pPresentInfo
);

void ctsDestroySwapchain(
    CtsDevice device,
    CtsSwapchain swapchain,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif