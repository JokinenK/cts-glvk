#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetStencilCompareMask {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkStencilFaceFlags faceMask;
    uint32_t compareMask;
} CtsCmdSetStencilCompareMask;

#ifdef __cplusplus
}
#endif