#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/image_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyImage {
    CtsCmdBase base;
    VkDevice device;
    VkImage image;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyImage;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsDestroyImage,
    ctsDestroyImageImpl,
    device,
    image,
    pAllocator
);

CTS_DEFINE_COMMAND_METADATA(CtsDestroyImage);

#ifdef __cplusplus
}
#endif