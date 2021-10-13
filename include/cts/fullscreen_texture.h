#pragma once

#include <stdint.h>
#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSurfaceHelper {
    GLenum shaders[2];
    GLuint shaderProgram;
    GLuint readFramebuffer;
    GLuint writeFramebuffer;
};

VkResult ctsInitSurfaceHelper(struct CtsSurfaceHelper* surfaceHelper);
void ctsDestroySurfaceHelper(struct CtsSurfaceHelper* surfaceHelper);

void ctsBlitTexture(
    struct CtsSurfaceHelper* surfaceHelper,
    struct CtsDevice* device,
    struct CtsImage* src,
    struct CtsImage* dst,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
);

void ctsDrawFSTexture(
    struct CtsSurfaceHelper* surfaceHelper,
    struct CtsDevice* device,
    struct CtsImage* image
);



#ifdef __cplusplus
}
#endif