#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/descriptor_set_private.h"

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

CTS_DEFINE_TRAMPOLINE(
    AllocateDescriptorSets, 
    device, 
    pAllocateInfo, 
    pDescriptorSets
);

#ifdef __cplusplus
}
#endif