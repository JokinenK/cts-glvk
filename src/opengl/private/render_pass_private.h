#pragma once

#include <glad/glad.h>
#include <cts/typedefs/render_pass.h>
#include <cts/typedefs/gl_render_pass.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsRenderPassImpl {
    CtsRenderPassCreateFlags flags;
    
    uint32_t attachmentCount;
    CtsAttachmentDescription* pAttachments;
    GLenum* pDrawBuffers;

    uint32_t subpassCount;
    CtsGlSubpassDescription* pSubpasses;
    
    uint32_t dependencyCount;
    CtsSubpassDependency* pDependencies;
};

#ifdef __cplusplus
}
#endif