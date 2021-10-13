#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdExecuteCommands {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t commandBufferCount;
    const VkCommandBuffer* pCommandBuffers;
} CtsCmdExecuteCommands;

#ifdef __cplusplus
}
#endif