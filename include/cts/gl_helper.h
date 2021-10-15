#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ctsInitGlHelper();
void ctsDestroyGlHelper();

void ctsGlHelperBlitTexture(
    struct CtsDevice* device,
    struct CtsImage* src,
    struct CtsImage* dst,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
);

void ctsGlHelperDrawFSTexture(
    struct CtsDevice* device,
    struct CtsImage* image
);

#ifdef __cplusplus
}
#endif