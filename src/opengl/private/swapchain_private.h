#pragma once

#include <stdint.h>
#include <cts/swapchain.h>
#include <cts/typedefs/swapchain.h>
#include <cts/typedefs/bool.h>
#include <cts/typedefs/device.h>
#include <cts/typedefs/image.h>
#include <cts/typedefs/fence.h>
#include <cts/typedefs/semaphore.h>
#include <cts/typedefs/surface.h>
#include <cts/typedefs/extension_properties.h>

#ifdef __cplusplus
extern "C" {
#endif

static const CtsExtensionProperties swapchainExtensionProperties = {
    .extensionName = CTS_SWAPCHAIN_EXTENSION_NAME,
    .specVersion = 1,
};

typedef struct CtsSwapchainEntry {
    CtsImage image;
    CtsSemaphore semaphore;
} CtsSwapchainEntry;

struct CtsSwapchainImpl {
    CtsDevice device;
    uint32_t entryCount;
    uint32_t nextEntry;
    CtsSwapchainEntry* pEntries;
    CtsExtent2D extent;
    CtsSurfaceCapabilities surfaceCapabilities;
};

CtsResult ctsCreateSwapchainImpl(
    CtsDevice device,
    const CtsSwapchainCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSwapchain* pSwapchain
);

void ctsDestroySwapchainImpl(
    CtsDevice device,
    CtsSwapchain swapchain,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsQueuePresentImpl(
    CtsQueue queue,
    const CtsPresentInfo* pPresentInfo
);

#ifdef __cplusplus
}
#endif