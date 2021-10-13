#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAllocateDescriptorSets {
    CtsCmdBase base;
    VkDevice device;
    const VkDescriptorSetAllocateInfo* pAllocateInfo;
    VkDescriptorSet* pDescriptorSets;
    VkResult* pResult;
} CtsAllocateDescriptorSets;

#ifdef __cplusplus
}
#endif