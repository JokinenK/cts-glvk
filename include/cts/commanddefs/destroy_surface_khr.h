#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySurfaceKHR {
    CtsCmdBase base;
    VkInstance instance;
    VkSurfaceKHR surface;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroySurfaceKHR;

#ifdef __cplusplus
}
#endif