#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBeginRenderPass {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkRenderPassBeginInfo* pRenderPassBegin;
    VkSubpassContents contents;
} CtsCmdBeginRenderPass;

#ifdef __cplusplus
}
#endif