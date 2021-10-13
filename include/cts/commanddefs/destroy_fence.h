#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyFence {
    CtsCmdBase base;
    VkDevice device;
    VkFence fence;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyFence;

#ifdef __cplusplus
}
#endif