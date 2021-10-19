#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/image_view_private.h"

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

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsCreateImageView, 
    ctsCreateImageViewImpl, 
    device,
    pCreateInfo,
    pAllocator,
    pImageView
);

CTS_DEFINE_COMMAND_METADATA(CtsCreateImageView);

#ifdef __cplusplus
}
#endif