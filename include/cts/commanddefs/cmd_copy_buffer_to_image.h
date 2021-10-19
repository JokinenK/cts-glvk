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

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdCopyBufferToImage, 
    ctsCmdCopyBufferToImageImpl, 
    commandBuffer, 
    srcBuffer, 
    dstImage,
    dstImageLayout,
    regionCount,
    pRegions
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdCopyBufferToImage);

#ifdef __cplusplus
}
#endif