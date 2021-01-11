#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateImageView(
    CtsDevice pDevice,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
);

void ctsDestroyImageView(
    CtsDevice pDevice,
    CtsImageView pImageView,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif