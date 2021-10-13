#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDispatch {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t groupCountX;
    uint32_t groupCountY;
    uint32_t groupCountZ;
} CtsCmdDispatch;

#ifdef __cplusplus
}
#endif