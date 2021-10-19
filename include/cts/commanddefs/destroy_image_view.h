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

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsDestroyImageView,
    ctsDestroyImageViewImpl,
    device,
    imageView,
    pAllocator
);

CTS_DEFINE_COMMAND_METADATA(CtsDestroyImageView);

#ifdef __cplusplus
}
#endif