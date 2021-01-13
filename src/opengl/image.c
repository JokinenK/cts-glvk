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
    CtsDevice device,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
) {
    CtsResult result;
    CtsCreateImage cmd;
    cmd.base.type = CTS_COMMAND_CREATE_IMAGE;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pImage = pImage;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatchSemaphore);
    ctsWaitSemaphores(1, &device->dispatchSemaphore);

    return result;
}

void ctsDestroyImage(
    CtsDevice device,
    CtsImage image,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyImage cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_IMAGE;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.image = image;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatchSemaphore);
    ctsWaitSemaphores(1, &device->dispatchSemaphore);
}

CtsResult ctsCreateImageImpl(
    CtsDevice device,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
) {
    (void) device;

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
    (void) pCreateInfo->pQueueFamilyIndices;
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
    CtsDevice device,
    CtsImage image,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

    if (image != NULL) {
        glDeleteTextures(1, &image->handle);
        ctsFree(pAllocator, image);
    }
}

#ifdef __cplusplus
}
#endif