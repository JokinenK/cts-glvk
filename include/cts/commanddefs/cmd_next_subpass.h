#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdNextSubpass {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkSubpassContents contents;
} CtsCmdNextSubpass;

#ifdef __cplusplus
}
#endif