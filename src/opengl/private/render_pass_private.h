#pragma once

#include <glad/glad.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsRenderPass {
    CtsRenderPassCreateFlags flags;
    
    uint32_t attachmentCount;
    CtsAttachmentDescription* attachments;
    
    uint32_t subpassCount;
    CtsSubpassDescription* subpasses;
    
    uint32_t dependencyCount;
    CtsSubpassDependency* dependencies;

    GLenum* drawBuffers;
};

#ifdef __cplusplus
}
#endif