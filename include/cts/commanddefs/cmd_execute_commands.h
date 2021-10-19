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

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdExecuteCommands, 
    ctsCmdExecuteCommandsImpl, 
    commandBuffer,
    commandBufferCount,
    pCommandBuffers
);

#ifdef __cplusplus
}
#endif