#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySampler {
    CtsCmdBase base;
    VkDevice device;
    VkSampler sampler;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroySampler;

#ifdef __cplusplus
}
#endif