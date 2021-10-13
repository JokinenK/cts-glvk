#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFramebuffer {
    GLuint handle;
    struct CtsRenderPass* renderPass;
    uint32_t attachmentCount;
    struct CtsImageView** ppAttachments;
    GLenum* types;
};

VkResult ctsCreateFramebufferImpl(
    VkDevice device,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer
);

void ctsDestroyFramebufferImpl(
    VkDevice device,
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif