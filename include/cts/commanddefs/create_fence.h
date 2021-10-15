#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/fence_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateFence {
    CtsCmdBase base;
    VkDevice device;
    const VkFenceCreateInfo* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkFence* pFence;
    VkResult* pResult;
} CtsCreateFence;

CTS_DEFINE_TRAMPOLINE(
    CreateFence, 
    device,
    pCreateInfo,
    pAllocator,
    pFence
);

#ifdef __cplusplus
}
#endif