#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"
#include <cts/typedefs/gl_render_pass.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsRenderPass {
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