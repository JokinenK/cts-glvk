#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyImage {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkImage srcImage;
    VkImageLayout srcImageLayout;
    VkImage dstImage;
    VkImageLayout dstImageLayout;
    uint32_t regionCount;
    const VkImageCopy* pRegions;
} CtsCmdCopyImage;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdCopyImage, 
    ctsCmdCopyImageImpl, 
    commandBuffer, 
    srcImage, 
    srcImageLayout,
    dstImage,
    dstImageLayout,
    regionCount,
    pRegions
);

#ifdef __cplusplus
}
#endif