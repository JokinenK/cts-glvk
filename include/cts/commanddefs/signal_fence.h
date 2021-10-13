#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSignalFence {
    CtsCmdBase base;
    VkDevice device;
    VkFence fence;
} CtsSignalFence;

#ifdef __cplusplus
}
#endif