#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "private/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipelineLayout {
    struct CtsObjectBase base;
    
    uint32_t setLayoutCount;
    struct CtsDescriptorSetLayout** ppSetLayouts;
    uint32_t pushConstantRangeCount;
    VkPushConstantRange* pushConstantRanges;
};

#ifdef __cplusplus
}
#endif