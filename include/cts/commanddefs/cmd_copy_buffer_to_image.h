#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyBufferToImage {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer srcBuffer;
    VkImage dstImage;
    VkImageLayout dstImageLayout;
    uint32_t regionCount;
    const VkBufferImageCopy* pRegions;
} CtsCmdCopyBufferToImage;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdCopyBufferToImage, 
    commandBuffer, 
    srcBuffer, 
    dstImage,
    dstImageLayout,
    regionCount,
    pRegions
);

#ifdef __cplusplus
}
#endif