#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsUpdateDescriptorSets {
    CtsCmdBase base;
    VkDevice device;
    uint32_t descriptorWriteCount;
    const VkWriteDescriptorSet* pDescriptorWrites;
    uint32_t descriptorCopyCount;
    const VkCopyDescriptorSet* pDescriptorCopies;
} CtsUpdateDescriptorSets;

#ifdef __cplusplus
}
#endif
