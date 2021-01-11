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

    const CtsImageCreateInfo* createInfo = pCreateInfo;

    (void) createInfo->mipLevels;
    (void) createInfo->arrayLayers;
    (void) createInfo->samples;
    //(void) createInfo->tiling;
    (void) createInfo->usage;
    //(void) createInfo->sharingMode;
    (void) createInfo->queueFamilyIndexCount;
    (void) createInfo->queueFamilyIndices;
    //(void) createInfo->initialLayout;

    CtsFormatPair formatPair = parseFormatPair(createInfo->imageFormat);

    glGenTextures(1, &image->handle);
    image->imageType = createInfo->imageType;
    image->target = parseImageType(createInfo->imageType);
    image->internalFormat = formatPair.internalFormat;
    glBindTexture(image->target, image->handle);

    // TODO: Support mip levels
    if (image->imageType == CTS_IMAGE_TYPE_1D) {
        glTexImage1D(
            image->target,
            0,
            image->internalFormat,
            createInfo->extent.width,
            0,
            formatPair.format,
            GL_RGBA,
            NULL
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_2D) {
        glTexImage2D(
            image->target,
            0,
            image->internalFormat,
            createInfo->extent.width,
            createInfo->extent.height,
            0,
            formatPair.format,
            GL_RGBA,
            NULL
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_3D) {
        glTexImage3D(
            image->target,
            0,
            image->internalFormat,
            createInfo->extent.width,
            createInfo->extent.height,
            createInfo->extent.depth,
            0,
            formatPair.format,
            GL_RGBA,
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