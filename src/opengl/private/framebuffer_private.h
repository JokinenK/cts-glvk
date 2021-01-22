#pragma once

#include <stdint.h>
#include <cts/typedefs/framebuffer.h>
#include <cts/typedefs/render_pass.h>
#include <cts/typedefs/image_view.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFramebufferImpl {
    GLuint handle;
    CtsRenderPass renderPass;
    uint32_t attachmentCount;
    CtsImageView* attachments;
};

CtsResult ctsCreateFramebufferImpl(
    CtsDevice device,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
);

void ctsDestroyFramebufferImpl(
    CtsDevice device,
    CtsFramebuffer framebuffer,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif