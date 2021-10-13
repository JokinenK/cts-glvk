#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsResetFences {
    CtsCmdBase base;
    VkDevice device;
    uint32_t fenceCount;
    const VkFence* pFences;
    VkResult* pResult;
} CtsResetFences;

#ifdef __cplusplus
}
#endif