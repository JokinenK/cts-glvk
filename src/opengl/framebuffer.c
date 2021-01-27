#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <cts/framebuffer.h>
#include <cts/commands.h>
#include <private/device_private.h>
#include <private/framebuffer_private.h>
#include <private/image_view_private.h>
#include <private/image_private.h>
#include <private/queue_private.h>
#include <private/render_pass_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFramebuffer(
    CtsDevice device,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
) {
    CtsResult result;
    CtsCreateFramebuffer cmd;
    cmd.base.type = CTS_COMMAND_CREATE_FRAMEBUFFER;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pFramebuffer = pFramebuffer;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void ctsDestroyFramebuffer(
    CtsDevice device,
    CtsFramebuffer framebuffer,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyFramebuffer cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_FRAMEBUFFER;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.framebuffer = framebuffer;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsCreateFramebufferImpl(
    CtsDevice device,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
) {
    (void) device;
    (void) pCreateInfo->flags;
    (void) pCreateInfo->layers;

    CtsFramebuffer framebuffer = ctsAllocation(
        pAllocator,
        sizeof(struct CtsFramebufferImpl),
        alignof(struct CtsFramebufferImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (framebuffer == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    glGenFramebuffers(1, &framebuffer->handle);
    framebuffer->renderPass = pCreateInfo->renderPass;
    framebuffer->attachmentCount = pCreateInfo->attachmentCount;
    framebuffer->attachments = ctsAllocation(
        pAllocator,
        sizeof(CtsImageView) * pCreateInfo->attachmentCount,
        alignof(CtsImageView),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );
    
    framebuffer->types = ctsAllocation(
        pAllocator,
        sizeof(GLenum) * pCreateInfo->attachmentCount,
        alignof(GLenum),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (framebuffer->attachments == NULL) {
        ctsDestroyFramebufferImpl(device, framebuffer, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        GLenum buffer = GL_NONE;
        CtsImageView attachment = pCreateInfo->pAttachments[i];

        if (attachment->aspectMask & CTS_IMAGE_ASPECT_COLOR_BIT) {
            buffer = GL_COLOR_ATTACHMENT0 + i;
        } else if (attachment->aspectMask & CTS_IMAGE_ASPECT_DEPTH_BIT && attachment->aspectMask & CTS_IMAGE_ASPECT_STENCIL_BIT) {
            buffer = GL_DEPTH_STENCIL_ATTACHMENT;
        } else if (attachment->aspectMask & CTS_IMAGE_ASPECT_DEPTH_BIT) {
            buffer = GL_DEPTH_ATTACHMENT;
        } else if (attachment->aspectMask & CTS_IMAGE_ASPECT_STENCIL_BIT) {
            buffer = GL_STENCIL_ATTACHMENT;
        }

        framebuffer->types[i] = buffer;
        framebuffer->attachments[i] = attachment;
    }

    *pFramebuffer = framebuffer;

    return CTS_SUCCESS;
}

void ctsDestroyFramebufferImpl(
    CtsDevice device,
    CtsFramebuffer framebuffer,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

    if (framebuffer != NULL) {
        glDeleteFramebuffers(1, &framebuffer->handle);

        ctsFree(pAllocator, framebuffer->attachments);
        ctsFree(pAllocator, framebuffer);
    }
}

#ifdef __cplusplus
}
#endif