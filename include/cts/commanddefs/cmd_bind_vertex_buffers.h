#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

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

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdBindVertexBuffers, 
    ctsCmdBindVertexBuffersImpl, 
    commandBuffer, 
    firstBinding, 
    bindingCount,
    pBuffers,
    pOffsets
);

#ifdef __cplusplus
}
#endif