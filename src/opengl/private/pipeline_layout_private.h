#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipelineLayout {
    uint32_t setLayoutCount;
    struct CtsDescriptorSetLayout** ppSetLayouts;
    uint32_t pushConstantRangeCount;
    VkPushConstantRange* pushConstantRanges;
};

#ifdef __cplusplus
}
#endif