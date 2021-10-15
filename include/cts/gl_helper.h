#pragma once

#include <stdint.h>
#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsGlHelper {
    GLenum shaders[2];
    GLuint shaderProgram;
    GLuint readFramebuffer;
    GLuint writeFramebuffer;
};

VkResult ctsInitGlHelper(struct CtsGlHelper* surfaceHelper);
void ctsDestroyGlHelper(struct CtsGlHelper* surfaceHelper);

void ctsGlHelperBlitTexture(
    struct CtsGlHelper* surfaceHelper,
    struct CtsDevice* device,
    struct CtsImage* src,
    struct CtsImage* dst,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
);

void ctsGlHelperDrawFSTexture(
    struct CtsGlHelper* surfaceHelper,
    struct CtsDevice* device,
    struct CtsImage* image
);



#ifdef __cplusplus
}
#endif