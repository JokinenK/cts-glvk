#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetBlendConstants {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    float blendConstants[4];
} CtsCmdSetBlendConstants;

#ifdef __cplusplus
}
#endif