#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdResetQueryPool {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkQueryPool queryPool;
    uint32_t firstQuery;
    uint32_t queryCount;
} CtsCmdResetQueryPool;

#ifdef __cplusplus
}
#endif