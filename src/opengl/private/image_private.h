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
};

CtsResult ctsCreateImageImpl(
    CtsDevice pDevice,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
);

void ctsDestroyImageImpl(
    CtsDevice pDevice,
    CtsImage pImage,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif
