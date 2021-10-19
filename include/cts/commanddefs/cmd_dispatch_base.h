#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDispatchBase {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t baseGroupX;
    uint32_t baseGroupY;
    uint32_t baseGroupZ;
    uint32_t groupCountX;
    uint32_t groupCountY;
    uint32_t groupCountZ;
} CtsCmdDispatchBase;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdDispatchBase, 
    ctsCmdDispatchBaseImpl, 
    commandBuffer,
    baseGroupX,
    baseGroupY,
    baseGroupZ,
    groupCountX, 
    groupCountY,
    groupCountZ
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdDispatchBase);

#ifdef __cplusplus
}
#endif