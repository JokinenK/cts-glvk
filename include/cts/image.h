#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateImage(
    CtsDevice device,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
);

void ctsDestroyImage(
    CtsDevice device,
    CtsImage image,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif