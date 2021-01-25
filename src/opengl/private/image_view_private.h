#pragma once

#include <cts/typedefs/image.h>
#include <cts/typedefs/image_view.h>
#include <cts/typedefs/sampler.h>
    
#ifdef __cplusplus
extern "C" {
#endif

struct CtsImageViewImpl {
    CtsImage image;
    CtsImageViewType viewType;
    CtsImageAspectFlags aspectMask;
    GLuint handle;
    GLenum target;
};

CtsResult ctsCreateImageViewImpl(
    CtsDevice device,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
);

void ctsDestroyImageViewImpl(
    CtsDevice device,
    CtsImageView imageView,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif
