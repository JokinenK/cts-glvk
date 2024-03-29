#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetLineWidth {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    float lineWidth;
} CtsCmdSetLineWidth;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdSetLineWidth, 
    ctsCmdSetLineWidthImpl, 
    commandBuffer,
    lineWidth
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdSetLineWidth);

#ifdef __cplusplus
}
#endif