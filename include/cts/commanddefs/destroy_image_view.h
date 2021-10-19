#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/image_view_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyImageView {
    CtsCmdBase base;
    VkDevice device;
    VkImageView imageView;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyImageView;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsDestroyImageView,
    ctsDestroyImageViewImpl,
    device,
    imageView,
    pAllocator
);

#ifdef __cplusplus
}
#endif