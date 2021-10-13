#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBeginQuery {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkQueryPool queryPool;
    uint32_t query;
    VkQueryControlFlags flags;
} CtsCmdBeginQuery;

#ifdef __cplusplus
}
#endif