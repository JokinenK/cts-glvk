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
    CtsDevice device,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
) {
    CtsResult result;
    CtsCreateImageView cmd;
    cmd.base.type = CTS_COMMAND_CREATE_IMAGE_VIEW;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pImageView = pImageView;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void ctsDestroyImageView(
    CtsDevice device,
    CtsImageView imageView,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyImageView cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_IMAGE_VIEW;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.imageView = imageView;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsCreateImageViewImpl(
    CtsDevice device,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
) {
    (void) device;

    CtsImageView imageView = ctsAllocation(
        pAllocator,
        sizeof(struct CtsImageViewImpl),
        alignof(struct CtsImageViewImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (imageView == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsFormatData formatData = parseFormat(pCreateInfo->format);
    (void) pCreateInfo->components;

    imageView->image = pCreateInfo->image;
    imageView->target = parseImageViewType(pCreateInfo->viewType);
    imageView->viewType = pCreateInfo->viewType;

    glGenTextures(1, &imageView->handle);
    glTextureView(
        imageView->handle,
        imageView->target,
        pCreateInfo->image->handle,
        formatData.internalFormat,
        pCreateInfo->subresourceRange.baseMipLevel,
        pCreateInfo->subresourceRange.levelCount,
        pCreateInfo->subresourceRange.baseArrayLayer,
        pCreateInfo->subresourceRange.layerCount
    );

    *pImageView = imageView;
    return CTS_SUCCESS;
}

void ctsDestroyImageViewImpl(
    CtsDevice device,
    CtsImageView imageView,
    const CtsAllocationCallbacks* pAllocator
) {
    if (imageView != NULL) {
        glDeleteTextures(1, &imageView->handle);
        ctsFree(pAllocator, imageView);
    }
}

#ifdef __cplusplus
}
#endif