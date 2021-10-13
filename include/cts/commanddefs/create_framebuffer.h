#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateFramebuffer {
    CtsCmdBase base;
    VkDevice device;
    const VkFramebufferCreateInfo* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkFramebuffer* pFramebuffer;
    VkResult* pResult;
} CtsCreateFramebuffer;

#ifdef __cplusplus
}
#endif