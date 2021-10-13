#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsGlDescriptorSetLayoutBinding {
    uint32_t binding;
    VkDescriptorType descriptorType;
    uint32_t descriptorCount;
    uint32_t descriptorOffset;
    VkShaderStageFlags stageFlags;
    const VkSampler* immutableSamplers;
};

struct CtsDescriptorSetLayout {
    uint32_t bindingCount;
    struct CtsGlDescriptorSetLayoutBinding* bindings;
};

#ifdef __cplusplus
}
#endif