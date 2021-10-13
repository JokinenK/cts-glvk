#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetDeviceMask {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t deviceMask;
} CtsCmdSetDeviceMask;

#ifdef __cplusplus
}
#endif