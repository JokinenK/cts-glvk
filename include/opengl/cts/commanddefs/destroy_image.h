#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyImage {
    CtsCmdBase base;
    VkDevice device;
    VkImage image;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyImage;

#ifdef __cplusplus
}
#endif