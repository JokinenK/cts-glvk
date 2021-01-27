#pragma once

#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateImageView(
    CtsDevice device,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
);

void ctsDestroyImageView(
    CtsDevice device,
    CtsImageView imageView,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif