#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/descriptor_set_private.h"

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

CTS_DEFINE_TRAMPOLINE(
    FreeDescriptorSets,
    device,
    descriptorPool,
    descriptorSetCount,
    pDescriptorSets
);

#ifdef __cplusplus
}
#endif