#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsInitFSTextureHelper();

void ctsBlitTexture(
    CtsDevice device,
    CtsImage src,
    CtsImage dst,
    uint32_t regionCount,
    const CtsImageBlit* pRegions,
    CtsFilter filter
);

void ctsDrawFSTexture(
    CtsDevice device,
    CtsImage image
);

void ctsCleanupFSTextureHelper();

#ifdef __cplusplus
}
#endif