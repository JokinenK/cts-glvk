#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateSampler(
    VkDevice device,
    const VkSamplerCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSampler* pSampler
);

void ctsDestroySampler(
    VkDevice device,
    VkSampler sampler,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif