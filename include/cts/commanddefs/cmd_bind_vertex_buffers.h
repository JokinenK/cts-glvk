#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindVertexBuffers {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t firstBinding;
    uint32_t bindingCount;
    const VkBuffer* pBuffers;
    const VkDeviceSize* pOffsets;
} CtsCmdBindVertexBuffers;

#ifdef __cplusplus
}
#endif