#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindIndexBuffer {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer buffer;
    VkDeviceSize offset;
    VkIndexType indexType;
} CtsCmdBindIndexBuffer;

#ifdef __cplusplus
}
#endif