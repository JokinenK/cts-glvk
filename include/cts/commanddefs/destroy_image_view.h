#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyImageView {
    CtsCmdBase base;
    VkDevice device;
    VkImageView imageView;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyImageView;

#ifdef __cplusplus
}
#endif