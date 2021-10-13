#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateImageView {
    CtsCmdBase base;
    VkDevice device;
    const VkImageViewCreateInfo* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkImageView* pImageView;
    VkResult* pResult;
} CtsCreateImageView;

#ifdef __cplusplus
}
#endif