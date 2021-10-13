#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdClearColorImage {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkImage image;
    VkImageLayout imageLayout;
    const VkClearColorValue* pColor;
    uint32_t rangeCount;
    const VkImageSubresourceRange* pRanges;
} CtsCmdClearColorImage;

#ifdef __cplusplus
}
#endif