#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <cts/framebuffer.h>
#include <cts/commands.h>
#include <private/private.h>
#include <private/device_private.h>
#include <private/framebuffer_private.h>
#include <private/image_view_private.h>
#include <private/image_private.h>
#include <private/queue_private.h>
#include <private/render_pass_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateFramebuffer(
    VkDevice deviceHandle,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsCreateFramebuffer cmd;
    cmd.base.type = CTS_COMMAND_CREATE_FRAMEBUFFER;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pFramebuffer = pFramebuffer;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void VKAPI_CALL ctsDestroyFramebuffer(
    VkDevice deviceHandle,
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsDestroyFramebuffer cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_FRAMEBUFFER;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.framebuffer = framebuffer;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult ctsCreateFramebufferImpl(
    VkDevice deviceHandle,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer
) {
    (void) pCreateInfo->flags;
    (void) pCreateInfo->layers;

    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsFramebuffer* framebuffer = ctsAllocation(
        pAllocator,
        sizeof(struct CtsFramebuffer),
        alignof(struct CtsFramebuffer),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (framebuffer == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(framebuffer);

    glGenFramebuffers(1, &framebuffer->handle);
    framebuffer->renderPass = CtsRenderPassFromHandle(pCreateInfo->renderPass);
    framebuffer->attachmentCount = pCreateInfo->attachmentCount;
    framebuffer->ppAttachments = ctsAllocation(
        pAllocator,
        sizeof(struct CtsImageView*) * pCreateInfo->attachmentCount,
        alignof(struct CtsImageView*),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );
    
    framebuffer->types = ctsAllocation(
        pAllocator,
        sizeof(GLenum) * pCreateInfo->attachmentCount,
        alignof(GLenum),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (framebuffer->ppAttachments == NULL) {
        ctsDestroyFramebufferImpl(deviceHandle, CtsFramebufferToHandle(framebuffer), pAllocator);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        GLenum buffer = GL_NONE;
        struct CtsImageView* attachment = CtsImageViewFromHandle(pCreateInfo->pAttachments[i]);

        if (attachment->aspectMask & VK_IMAGE_ASPECT_COLOR_BIT) {
            buffer = GL_COLOR_ATTACHMENT0 + i;
        } else if (attachment->aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT && attachment->aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) {
            buffer = GL_DEPTH_STENCIL_ATTACHMENT;
        } else if (attachment->aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) {
            buffer = GL_DEPTH_ATTACHMENT;
        } else if (attachment->aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) {
            buffer = GL_STENCIL_ATTACHMENT;
        }

        framebuffer->types[i] = buffer;
        framebuffer->ppAttachments[i] = attachment;
    }

    *pFramebuffer = CtsFramebufferToHandle(framebuffer);

    return VK_SUCCESS;
}

void ctsDestroyFramebufferImpl(
    VkDevice deviceHandle,
    VkFramebuffer framebufferHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;
    
    struct CtsFramebuffer* framebuffer = CtsFramebufferFromHandle(framebufferHandle);

    if (framebuffer != NULL) {
        glDeleteFramebuffers(1, &framebuffer->handle);

        ctsFree(pAllocator, framebuffer->ppAttachments);
        ctsFree(pAllocator, framebuffer);
    }
}

#ifdef __cplusplus
}
#endif