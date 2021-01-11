#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateImage(
    CtsDevice pDevice,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
);

void ctsDestroyImage(
    CtsDevice pDevice,
    CtsImage pImage,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif