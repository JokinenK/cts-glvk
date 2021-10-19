#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetDepthBias {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
} CtsCmdSetDepthBias;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdSetDepthBias, 
    ctsCmdSetDepthBiasImpl, 
    commandBuffer,
    depthBiasConstantFactor,
    depthBiasClamp,
    depthBiasSlopeFactor
);

#ifdef __cplusplus
}
#endif