#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"
#include "cts/gl_render_pass.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsRenderPass {
    struct CtsObjectBase base;
    
    VkRenderPassCreateFlags flags;
    
    uint32_t attachmentCount;
    VkAttachmentDescription* pAttachments;
    GLenum* pDrawBuffers;

    uint32_t subpassCount;
    CtsGlSubpassDescription* pSubpasses;
    
    uint32_t dependencyCount;
    VkSubpassDependency* pDependencies;
};

#ifdef __cplusplus
}
#endif