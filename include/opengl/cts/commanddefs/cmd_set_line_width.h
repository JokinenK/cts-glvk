#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetLineWidth {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    float lineWidth;
} CtsCmdSetLineWidth;

#ifdef __cplusplus
}
#endif