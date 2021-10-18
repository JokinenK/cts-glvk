#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

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

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdClearDepthStencilImage, 
    commandBuffer, 
    image, 
    imageLayout,
    pDepthStencil,
    rangeCount,
    pRanges
);

#ifdef __cplusplus
}
#endif