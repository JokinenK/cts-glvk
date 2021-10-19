#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/descriptor_set_private.h"

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

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsUpdateDescriptorSets,
    ctsUpdateDescriptorSetsImpl,
    device,
    descriptorWriteCount,
    pDescriptorWrites,
    descriptorCopyCount,
    pDescriptorCopies
);

CTS_DEFINE_COMMAND_METADATA(CtsUpdateDescriptorSets);

#ifdef __cplusplus
}
#endif
