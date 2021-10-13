#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdEndQuery {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkQueryPool queryPool;
    uint32_t query;
} CtsCmdEndQuery;

#ifdef __cplusplus
}
#endif