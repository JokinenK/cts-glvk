#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/fence_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyFence {
    CtsCmdBase base;
    VkDevice device;
    VkFence fence;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyFence;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsDestroyFence,
    ctsDestroyFenceImpl,
    device,
    fence,
    pAllocator
);

CTS_DEFINE_COMMAND_METADATA(CtsDestroyFence);

#ifdef __cplusplus
}
#endif