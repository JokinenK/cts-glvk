#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetScissor {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t firstScissor;
    uint32_t scissorCount;
    const VkRect2D* pScissors;
} CtsCmdSetScissor;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdSetScissor, 
    ctsCmdSetScissorImpl, 
    commandBuffer,
    firstScissor,
    scissorCount,
    pScissors
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdSetScissor);

#ifdef __cplusplus
}
#endif