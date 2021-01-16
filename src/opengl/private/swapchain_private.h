#pragma once

#include <stdint.h>
#include <cts/swapchain.h>
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

const CtsExtensionProperties swapchainExtensionProperties = {
    .extensionName = CTS_SWAPCHAIN_EXTENSION_NAME,
    .specVersion = 1,
};

typedef struct CtsSwapchainEntry {
    CtsImage image;
    CtsFence fence;
    CtsSemaphore semaphore;
    CtsBool32 free;
} CtsSwapchainEntry;

struct CtsSwapchain {
    CtsDevice device;
    uint32_t entryCount;
    CtsSwapchainEntry* pEntries;
};

#ifdef __cplusplus
}
#endif