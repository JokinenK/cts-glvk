#include <assert.h>
#include <stddef.h>
#include <cts/framebuffer.h>
#include <cts/commands.h>
#include <private/device_private.h>
#include <private/framebuffer_private.h>
#include <private/image_view_private.h>
#include <private/queue_private.h>
#include <private/render_pass_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFramebuffer(
    CtsDevice pDevice,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
) {
    CtsResult result;
    CtsCreateFramebuffer cmd;
    cmd.base.type = CTS_COMMAND_WAIT_FOR_FENCES;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.framebuffer = pFramebuffer;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

void ctsDestroyFramebuffer(
    CtsDevice pDevice,
    CtsFramebuffer pFramebuffer,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyFramebuffer cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_FENCE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.framebuffer = pFramebuffer;
    cmd.allocator = pAllocator;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

CtsResult ctsCreateFramebufferImpl(
    CtsDevice pDevice,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
) {
    (void) pDevice;
    (void) pCreateInfo->flags;
    (void) pCreateInfo->width;
    (void) pCreateInfo->height;
    (void) pCreateInfo->layers;

    CtsFramebuffer framebuffer = ctsAllocation(
        pAllocator,
        sizeof(struct CtsFramebuffer),
        alignof(struct CtsFramebuffer),
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
    
    if (framebuffer->attachments == NULL) {
        ctsDestroyFramebufferImpl(pDevice, framebuffer, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    framebuffer->drawBuffers = ctsAllocation(
        pAllocator,
        sizeof(GLenum) * pCreateInfo->attachmentCount,
        alignof(GLenum),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (framebuffer->drawBuffers == NULL) {
        ctsDestroyFramebufferImpl(pDevice, framebuffer, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        framebuffer->attachments[i] = pCreateInfo->attachments[i];
    }

    *pFramebuffer = framebuffer;
    return CTS_SUCCESS;
}

void ctsDestroyFramebufferImpl(
    CtsDevice pDevice,
    CtsFramebuffer pFramebuffer,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pFramebuffer != NULL) {
        glDeleteFramebuffers(1, &pFramebuffer->handle);

        ctsFree(pAllocator, pFramebuffer->attachments);
        ctsFree(pAllocator, pFramebuffer->drawBuffers);
        ctsFree(pAllocator, pFramebuffer);
    }
}

#ifdef __cplusplus
}
#endif