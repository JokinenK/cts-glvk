#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdClearDepthStencilImage {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkImage image;
    VkImageLayout imageLayout;
    const VkClearDepthStencilValue* pDepthStencil;
    uint32_t rangeCount;
    const VkImageSubresourceRange* pRanges;
} CtsCmdClearDepthStencilImage;

#ifdef __cplusplus
}
#endif