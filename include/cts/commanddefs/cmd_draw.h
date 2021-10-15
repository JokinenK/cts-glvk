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

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdDraw, 
    commandBuffer, 
    vertexCount, 
    instanceCount,
    firstVertex,
    firstInstance
);

#ifdef __cplusplus
}
#endif