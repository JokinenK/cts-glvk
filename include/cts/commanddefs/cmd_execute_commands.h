#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdExecuteCommands {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t commandBufferCount;
    const VkCommandBuffer* pCommandBuffers;
} CtsCmdExecuteCommands;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdExecuteCommands, 
    ctsCmdExecuteCommandsImpl, 
    commandBuffer,
    commandBufferCount,
    pCommandBuffers
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdExecuteCommands);

#ifdef __cplusplus
}
#endif