#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

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

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdClearColorImage, 
    ctsCmdClearColorImageImpl, 
    commandBuffer, 
    image, 
    imageLayout,
    pColor,
    rangeCount,
    pRanges
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdClearColorImage);

#ifdef __cplusplus
}
#endif