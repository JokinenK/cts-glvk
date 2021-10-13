#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateSampler {
    CtsCmdBase base;
    VkDevice device;
    const VkSamplerCreateInfo* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkSampler* pSampler;
    VkResult* pResult;
} CtsCreateSampler;

#ifdef __cplusplus
}
#endif