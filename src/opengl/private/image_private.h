#pragma once

#include <glad/glad.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/image.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsImageImpl {
    GLenum handle;
    GLenum format;
    GLenum internalFormat;
    GLenum type;
    GLenum target;
    CtsImageType imageType;
    CtsImageUsageFlags imageUsage;

    GLint width;
    GLint height;
    GLint depth;
    GLint arrayLayers;
    GLint mipLevels;
    GLint numComponents;

    CtsDeviceSize size;
    CtsDeviceSize offset;
    CtsImage nextSibling;
    CtsDeviceMemory memory;
};

CtsResult ctsCreateImageImpl(
    CtsDevice device,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
);

void ctsDestroyImageImpl(
    CtsDevice device,
    CtsImage image,
    const CtsAllocationCallbacks* pAllocator
);

uintptr_t ctsImageToBuffer(CtsImage image, uintptr_t offset);
uintptr_t ctsBufferToImage(CtsImage image, uintptr_t offset);

#ifdef __cplusplus
}
#endif
