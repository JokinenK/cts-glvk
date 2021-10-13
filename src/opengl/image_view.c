#include <stddef.h>
#include <cts/commands.h>
#include <cts/image_view.h>
#include <cts/type_mapper.h>
#include <private/private.h>
#include <private/device_private.h>
#include <private/image_view_private.h>
#include <private/image_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateImageView(
    VkDevice deviceHandle,
    const VkImageViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImageView* pImageView
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsCreateImageView cmd;
    cmd.base.type = CTS_COMMAND_CREATE_IMAGE_VIEW;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pImageView = pImageView;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void ctsDestroyImageView(
    VkDevice deviceHandle,
    VkImageView imageView,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsDestroyImageView cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_IMAGE_VIEW;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.imageView = imageView;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult ctsCreateImageViewImpl(
    VkDevice deviceHandle,
    const VkImageViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImageView* pImageView
) {
    (void) deviceHandle;

    struct CtsImageView* imageView = ctsAllocation(
        pAllocator,
        sizeof(struct CtsImageView),
        alignof(struct CtsImageView),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (imageView == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    struct CtsImage* image = CtsImageFromHandle(pCreateInfo->image);
    bool isMultisampled = (image->samples > 1);

    const CtsFormatData* formatData = parseFormat(pCreateInfo->format);
    (void) pCreateInfo->components;

    imageView->image = CtsImageFromHandle(pCreateInfo->image);
    imageView->target = parseImageViewType(pCreateInfo->viewType, isMultisampled);
    imageView->viewType = pCreateInfo->viewType;
    imageView->aspectMask = pCreateInfo->subresourceRange.aspectMask;

    glGenTextures(1, &imageView->handle);
    glTextureView(
        imageView->handle,
        imageView->target,
        image->handle,
        formatData->internalFormat,
        pCreateInfo->subresourceRange.baseMipLevel,
        pCreateInfo->subresourceRange.levelCount,
        pCreateInfo->subresourceRange.baseArrayLayer,
        pCreateInfo->subresourceRange.layerCount
    );

    *pImageView = CtsImageViewToHandle(imageView);
    return VK_SUCCESS;
}

void ctsDestroyImageViewImpl(
    VkDevice deviceHandle,
    VkImageView imageViewHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsImageView* imageView = CtsImageViewFromHandle(imageViewHandle);

    if (imageView != NULL) {
        glDeleteTextures(1, &imageView->handle);
        ctsFree(pAllocator, imageView);
    }
}

#ifdef __cplusplus
}
#endif