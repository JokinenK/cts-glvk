#pragma once

#include <glad/glad.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsRenderPass {
    CtsRenderPassCreateFlags flags;
    
    uint32_t attachmentCount;
    CtsAttachmentDescription* pAttachments;
    
    uint32_t subpassCount;
    CtsSubpassDescription* pSubpasses;
    
    uint32_t dependencyCount;
    CtsSubpassDependency* pDependencies;

    GLenum* pDrawBuffers;
};

#ifdef __cplusplus
}
#endif