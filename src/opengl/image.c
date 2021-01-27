#include <stdint.h>
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

static uint32_t max(uint32_t a, uint32_t b);
static uint32_t nextMipValue(uint32_t value);
static CtsDeviceSize imageSize(CtsImage image);

static void storage1D(GLenum target, GLsizei mipLevels, GLenum internalFormat, GLsizei width, GLsizei arrayLayers, GLsizei samples);
static void storage2D(GLenum target, GLsizei mipLevels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei arrayLayers, GLsizei samples);
static void storage3D(GLenum target, GLsizei mipLevels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLsizei arrayLayers, GLsizei samples);

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

    ctsQueueDispatch(device->queue, &cmd.base);

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

    ctsQueueDispatch(device->queue, &cmd.base);
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
        sizeof(struct CtsImageImpl),
        alignof(struct CtsImageImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (image == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    (void) pCreateInfo->samples;
    //(void) pCreateInfo->tiling;
    (void) pCreateInfo->sharingMode;
    (void) pCreateInfo->queueFamilyIndexCount;
    (void) pCreateInfo->pQueueFamilyIndices;
    (void) pCreateInfo->initialLayout;

    bool isArray = pCreateInfo->arrayLayers > 1;
    bool isMultisampled = pCreateInfo->samples > CTS_SAMPLE_COUNT_1_BIT;
    CtsFormatData formatData = parseFormat(pCreateInfo->format);
    
    glGenTextures(1, &image->handle);
    image->imageType = pCreateInfo->imageType;
    image->target = parseImageType(pCreateInfo->imageType, isMultisampled, isArray);
    image->format = formatData.format;
    image->internalFormat = formatData.internalFormat;
    image->type = formatData.type;
    image->imageUsage = pCreateInfo->usage;
    image->offset = 0;
    image->nextSibling = NULL;
    image->memory = NULL;
    image->numComponents = formatData.numComponents;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(image->target, image->handle);

    // TODO: Support mip levels
    if (image->imageType == CTS_IMAGE_TYPE_1D) {
        image->width = pCreateInfo->extent.width;
        image->height = 1;
        image->depth = 1;
        image->arrayLayers = pCreateInfo->arrayLayers;
        image->mipLevels = pCreateInfo->mipLevels;
        image->samples = pCreateInfo->samples;
        image->size = imageSize(image);

        storage1D(
            image->target,
            image->mipLevels,
            image->internalFormat,
            image->width,
            image->arrayLayers,
            image->samples
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_2D) {
        image->width = pCreateInfo->extent.width;
        image->height = pCreateInfo->extent.height;
        image->depth = 1;
        image->arrayLayers = pCreateInfo->arrayLayers;
        image->mipLevels = pCreateInfo->mipLevels;
        image->samples = pCreateInfo->samples;
        image->size = imageSize(image);

        storage2D(
            image->target,
            image->mipLevels,
            image->internalFormat,
            image->width,
            image->height,
            image->arrayLayers,
            image->samples
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_3D) {
        image->width = pCreateInfo->extent.width;
        image->height = pCreateInfo->extent.height;
        image->depth = pCreateInfo->extent.depth;
        image->arrayLayers = pCreateInfo->arrayLayers;
        image->mipLevels = pCreateInfo->mipLevels;
        image->samples = pCreateInfo->samples;
        image->size = imageSize(image);

        storage3D(
            image->target,
            image->mipLevels,
            image->internalFormat,
            image->width,
            image->height,
            image->depth,
            image->arrayLayers,
            image->samples
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

static uint32_t max(uint32_t a, uint32_t b) {
    return a > b ? a : b;
}

static uint32_t nextMipValue(uint32_t value) {
    return max(value / 2, 1);
}

static CtsDeviceSize imageSize(CtsImage image) {
    CtsDeviceSize size = 0;

    uint32_t width = image->width;
    uint32_t height = image->height;
    uint32_t depth = image->depth;

    for (GLint i = 0; i < image->mipLevels; ++i) {
        size += width * height * depth * image->arrayLayers * image->numComponents * image->samples;
        width = nextMipValue(width);
        height = nextMipValue(height);
        depth = nextMipValue(depth);

        if (width == 1 && height == 1 && depth == 1) {
            break;
        }
    }

    return size;
}

uintptr_t ctsImageToBuffer(CtsImage image, uintptr_t offset) {
    uint32_t width = image->width;
    uint32_t height = image->height;
    uint32_t depth = image->depth;

    for (GLint i = 0; i < image->mipLevels; ++i) {
        glGetTexImage(
            image->target,                 
            i,
            image->format,
            image->type,
            (GLvoid*)offset
        );

        offset += width * height * depth * image->numComponents;
        width = nextMipValue(width);
        height = nextMipValue(height);
        depth = nextMipValue(depth);
    }

    return offset;
}

uintptr_t ctsBufferToImage(CtsImage image, uintptr_t offset) {
    uint32_t width = image->width;
    uint32_t height = image->height;
    uint32_t depth = image->depth;

    switch (image->target) {
        case GL_TEXTURE_1D: {
            for (GLint level = 0; level < image->mipLevels; ++level) {
                glTexSubImage1D(image->target, level, 0, width, image->format, image->type, (GLvoid*)offset);
                offset += width * image->numComponents;
                width = nextMipValue(width);
            }
        } break;

        case GL_TEXTURE_2D: {
            for (GLint level = 0; level < image->mipLevels; ++level) {
                glTexSubImage2D(image->target, level, 0, 0, width, height, image->format, image->type, (GLvoid*)offset);
                offset += width * height * image->numComponents;
                width = nextMipValue(width);
                height = nextMipValue(height);
            }
        } break;

        case GL_TEXTURE_1D_ARRAY: {
            for (GLint level = 0; level < image->mipLevels; ++level) {
                glTexSubImage2D(image->target, level, 0, 0, width, image->arrayLayers, image->format, image->type, (GLvoid*)offset);
                offset += width * image->arrayLayers * image->numComponents;
                width = nextMipValue(width);
            }
        } break;

        case GL_TEXTURE_3D: {
            for (GLint level = 0; level < image->mipLevels; ++level) {
                glTexSubImage3D(image->target, level, 0, 0, 0, width, height, depth, image->format, image->type, (GLvoid*)offset);
                offset += width * height * depth * image->numComponents;
                width = nextMipValue(width);
                height = nextMipValue(height);
                depth = nextMipValue(depth);
            }
        } break;

        case GL_TEXTURE_2D_ARRAY: {
            for (GLint level = 0; level < image->mipLevels; ++level) {
                glTexSubImage3D(image->target, level, 0, 0, 0, width, height, image->arrayLayers, image->format, image->type, (GLvoid*)offset);
                offset += width * height * image->arrayLayers * image->numComponents;
                width = nextMipValue(width);
                height = nextMipValue(height);
            }
        } break;
    }

    return offset;
}

static void storage1D(GLenum target, GLsizei mipLevels, GLenum internalFormat, GLsizei width, GLsizei arrayLayers, GLsizei samples)
{
    bool isArray = arrayLayers > 1;
    bool isMultisampled = samples > 1;

    if (!isMultisampled) {
        if (!isArray) {
            glTexStorage1D(target, mipLevels, internalFormat, width);
        } else {
            glTexStorage2D(target, mipLevels, internalFormat, width, arrayLayers);
        }
    } else {
        // Unsupported
    }
}

static void storage2D(GLenum target, GLsizei mipLevels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei arrayLayers, GLsizei samples)
{
    bool isArray = arrayLayers > 1;
    bool isMultisampled = samples > 1;

    if (!isMultisampled) {
        if (!isArray) {
            glTexStorage2D(target, mipLevels, internalFormat, width, height);
        } else {
            glTexStorage3D(target, mipLevels, internalFormat, width, height, arrayLayers);
        }
    } else {
        if (!isArray) {
            glTexStorage2DMultisample(target, samples, internalFormat, width, height, GL_TRUE);
        } else {
            glTexStorage3DMultisample(target, samples, internalFormat, width, height, arrayLayers, GL_TRUE);
        }
    }
}

static void storage3D(GLenum target, GLsizei mipLevels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLsizei arrayLayers, GLsizei samples)
{
    bool isArray = arrayLayers > 1;
    bool isMultisampled = samples > 1;

    if (!isMultisampled) {
        if (!isArray) {
            glTexStorage3D(target, mipLevels, internalFormat, width, height, depth);
        } else {
            // Unsupported
        }
    } else {
        if (!isArray) {
            glTexStorage3DMultisample(target, samples, internalFormat, width, height, arrayLayers, GL_TRUE);
        } else {
            // Unsupported
        }
    }
}

#ifdef __cplusplus
}
#endif