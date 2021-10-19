#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdPushConstants {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkPipelineLayout layout;
    VkShaderStageFlags stageFlags;
    uint32_t offset;
    uint32_t size;
    const void* pValues;
} CtsCmdPushConstants;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdPushConstants, 
    ctsCmdPushConstantsImpl, 
    commandBuffer,
    layout,
    stageFlags,
    offset,
    size,
    pValues
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdPushConstants);

#ifdef __cplusplus
}
#endif