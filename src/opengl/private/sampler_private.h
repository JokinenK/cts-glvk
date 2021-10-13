#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSampler {
    GLuint handle;
};

VkResult ctsCreateSamplerImpl(
    VkDevice device,
    const VkSamplerCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSampler* pSampler
);

void ctsDestroySamplerImpl(
    VkDevice device,
    VkSampler sampler,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif