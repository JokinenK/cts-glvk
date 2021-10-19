#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/framebuffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyFramebuffer {
    CtsCmdBase base;
    VkDevice device;
    VkFramebuffer framebuffer;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyFramebuffer;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsDestroyFramebuffer,
    ctsDestroyFramebufferImpl,
    device,
    framebuffer,
    pAllocator
);

CTS_DEFINE_COMMAND_METADATA(CtsDestroyFramebuffer);

#ifdef __cplusplus
}
#endif