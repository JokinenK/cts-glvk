#pragma once

#include <glad/glad.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsImage {
    GLenum handle;
    GLenum internalFormat;
    GLenum type;
    GLenum target;
    CtsImageType imageType;
    CtsImageUsageFlags imageUsage;
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

#ifdef __cplusplus
}
#endif
