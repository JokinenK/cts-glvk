#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateImage {
    CtsCmdBase base;
    VkDevice device;
    const VkImageCreateInfo* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkImage* pImage;
    VkResult* pResult;
} CtsCreateImage;

#ifdef __cplusplus
}
#endif