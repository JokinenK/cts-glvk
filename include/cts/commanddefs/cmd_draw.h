#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDraw {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
} CtsCmdDraw;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdDraw, 
    ctsCmdDrawImpl, 
    commandBuffer, 
    vertexCount, 
    instanceCount,
    firstVertex,
    firstInstance
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdDraw);

#ifdef __cplusplus
}
#endif