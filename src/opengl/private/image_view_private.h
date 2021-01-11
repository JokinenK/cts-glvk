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
};

CtsResult ctsCreateImageViewImpl(
    CtsDevice pDevice,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
);

void ctsDestroyImageViewImpl(
    CtsDevice pDevice,
    CtsImageView pImageView,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif
