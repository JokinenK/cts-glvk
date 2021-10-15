#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/framebuffer_private.h"

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

CTS_DEFINE_TRAMPOLINE(
    CreateFramebuffer, 
    device,
    pCreateInfo,
    pAllocator,
    pFramebuffer
);

#ifdef __cplusplus
}
#endif