#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsInitFSTextureHelper();

void ctsBlitTexture(
    struct CtsDevice* device,
    struct CtsImage* src,
    struct CtsImage* dst,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
);

void ctsDrawFSTexture(
    struct CtsDevice* device,
    struct CtsImage* image
);

void ctsCleanupFSTextureHelper();

#ifdef __cplusplus
}
#endif