#include <stddef.h>
#include <cts/commands.h>
#include <cts/image.h>
#include <cts/type_mapper.h>
#include <private/device_private.h>
#include <private/image_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateImage(
    CtsDevice pDevice,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
) {
    CtsResult result;
    CtsCreateImage cmd;
    cmd.base.type = CTS_COMMAND_CREATE_IMAGE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.image = pImage;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

void ctsDestroyImage(
    CtsDevice pDevice,
    CtsImage pImage,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyImage cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_IMAGE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.image = pImage;
    cmd.allocator = pAllocator;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

CtsResult ctsCreateImageImpl(
    CtsDevice pDevice,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
) {
    (void) pDevice;

    CtsImage image = ctsAllocation(
        pAllocator,
        sizeof(struct CtsImage),
        alignof(struct CtsImage),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (image == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    (void) pCreateInfo->mipLevels;
    (void) pCreateInfo->arrayLayers;
    (void) pCreateInfo->samples;
    //(void) pCreateInfo->tiling;
    //(void) pCreateInfo->sharingMode;
    (void) pCreateInfo->queueFamilyIndexCount;
    (void) pCreateInfo->queueFamilyIndices;
    //(void) pCreateInfo->initialLayout;

    CtsFormatData formatData = parseFormat(pCreateInfo->imageFormat);

    glGenTextures(1, &image->handle);
    image->imageType = pCreateInfo->imageType;
    image->target = parseImageType(pCreateInfo->imageType);
    image->internalFormat = formatData.internalFormat;
    image->imageUsage = pCreateInfo->usage;
    glBindTexture(image->target, image->handle);

    // TODO: Support mip levels
    if (image->imageType == CTS_IMAGE_TYPE_1D) {
        glTexImage1D(
            image->target,
            0,
            formatData.internalFormat,
            pCreateInfo->extent.width,
            0,
            formatData.format,
            formatData.type,
            NULL
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_2D) {
        glTexImage2D(
            image->target,
            0,
            formatData.internalFormat,
            pCreateInfo->extent.width,
            pCreateInfo->extent.height,
            0,
            formatData.format,
            formatData.type,
            NULL
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_3D) {
        glTexImage3D(
            image->target,
            0,
            formatData.internalFormat,
            pCreateInfo->extent.width,
            pCreateInfo->extent.height,
            pCreateInfo->extent.depth,
            0,
            formatData.format,
            formatData.type,
            NULL
        );
    }

    glBindTexture(image->target, 0);

    *pImage = image;
    return CTS_SUCCESS;
}

void ctsDestroyImageImpl(
    CtsDevice pDevice,
    CtsImage pImage,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pImage != NULL) {
        glDeleteTextures(1, &pImage->handle);
        ctsFree(pAllocator, pImage);
    }
}

#ifdef __cplusplus
}
#endif