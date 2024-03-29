#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/image_private.h"

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

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsCreateImage,
    ctsCreateImageImpl,
    device,
    pCreateInfo,
    pAllocator,
    pImage
);

CTS_DEFINE_COMMAND_METADATA(CtsCreateImage);

#ifdef __cplusplus
}
#endif