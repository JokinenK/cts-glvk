#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDrawIndexed {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
} CtsCmdDrawIndexed;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdDrawIndexed, 
    ctsCmdDrawIndexedImpl,
    commandBuffer, 
    indexCount, 
    instanceCount,
    firstIndex,
    vertexOffset,
    firstInstance
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdDrawIndexed);

#ifdef __cplusplus
}
#endif