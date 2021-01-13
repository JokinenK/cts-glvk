#pragma once

#include <cts/typedefs/image.h>
#include <cts/typedefs/sampler.h>
    
#ifdef __cplusplus
extern "C" {
#endif

struct CtsImageView {
    GLenum target;
    GLenum handle;

    CtsImageViewType viewType;
    CtsImageUsageFlags imageUsage;
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
