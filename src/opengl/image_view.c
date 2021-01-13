#include <stddef.h>
#include <cts/commands.h>
#include <cts/image_view.h>
#include <cts/type_mapper.h>
#include <private/device_private.h>
#include <private/image_view_private.h>
#include <private/image_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateImageView(
    CtsDevice pDevice,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
) {
    CtsResult result;
    CtsCreateImageView cmd;
    cmd.base.type = CTS_COMMAND_CREATE_IMAGE_VIEW;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.imageView = pImageView;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

void ctsDestroyImageView(
    CtsDevice pDevice,
    CtsImageView pImageView,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyImageView cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_IMAGE_VIEW;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.imageView = pImageView;
    cmd.allocator = pAllocator;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

CtsResult ctsCreateImageViewImpl(
    CtsDevice pDevice,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
) {
    (void) pDevice;

    CtsImageView imageView = ctsAllocation(
        pAllocator,
        sizeof(struct CtsImageView),
        alignof(struct CtsImageView),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (imageView == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsFormatData formatData = parseFormat(pCreateInfo->format);
    (void) pCreateInfo->components;

    glGenTextures(1, &imageView->handle);
    glTextureView(
        imageView->handle,
        pCreateInfo->image->target,
        pCreateInfo->image->handle,
        formatData.internalFormat,
        pCreateInfo->subresourceRange.baseMipLevel,
        pCreateInfo->subresourceRange.levelCount,
        pCreateInfo->subresourceRange.baseArrayLayer,
        pCreateInfo->subresourceRange.layerCount
    );

    imageView->imageUsage = pCreateInfo->image->imageUsage;
    imageView->viewType = pCreateInfo->viewType;

    *pImageView = imageView;
    return CTS_SUCCESS;
}

void ctsDestroyImageViewImpl(
    CtsDevice pDevice,
    CtsImageView pImageView,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pImageView != NULL) {
        glDeleteTextures(1, &pImageView->handle);
        ctsFree(pAllocator, pImageView);
    }
}

#ifdef __cplusplus
}
#endif