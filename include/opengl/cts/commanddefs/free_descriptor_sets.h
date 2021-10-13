#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFreeDescriptorSets {
    CtsCmdBase base;
    VkDevice device;
    VkDescriptorPool descriptorPool;
    uint32_t descriptorSetCount;
    const VkDescriptorSet* pDescriptorSets;
    VkResult* pResult;
} CtsFreeDescriptorSets;

#ifdef __cplusplus
}
#endif