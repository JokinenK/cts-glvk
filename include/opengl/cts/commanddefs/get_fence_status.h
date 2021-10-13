#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGetFenceStatus {
    CtsCmdBase base;
    VkDevice device;
    VkFence fence;
    VkResult* pResult;
} CtsGetFenceStatus;

#ifdef __cplusplus
}
#endif