#pragma once

#include <stdint.h>
#include <cts/typedefs/render_pass.h>
#include <cts/typedefs/image_view.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFramebuffer {
    GLuint handle;
    CtsRenderPass renderPass;
    uint32_t attachmentCount;
    CtsImageView* attachments;
    GLenum* drawBuffers;
};

CtsResult ctsCreateFramebufferImpl(
    CtsDevice pDevice,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
);

void ctsDestroyFramebufferImpl(
    CtsDevice pDevice,
    CtsFramebuffer pFramebuffer,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif