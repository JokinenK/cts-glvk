#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyFramebuffer {
    CtsCmdBase base;
    VkDevice device;
    VkFramebuffer framebuffer;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyFramebuffer;

#ifdef __cplusplus
}
#endif