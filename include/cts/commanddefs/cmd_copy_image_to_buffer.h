#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyImageToBuffer {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkImage srcImage;
    VkImageLayout srcImageLayout;
    VkBuffer dstBuffer;
    uint32_t regionCount;
    const VkBufferImageCopy* pRegions;
} CtsCmdCopyImageToBuffer;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdCopyImageToBuffer, 
    ctsCmdCopyImageToBufferImpl, 
    commandBuffer, 
    srcImage, 
    srcImageLayout,
    dstBuffer,
    regionCount,
    pRegions
);

#ifdef __cplusplus
}
#endif