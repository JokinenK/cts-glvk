#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetBlendConstants {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    float blendConstants[4];
} CtsCmdSetBlendConstants;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdSetBlendConstants, 
    commandBuffer,
    blendConstants
);

#ifdef __cplusplus
}
#endif