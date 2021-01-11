#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsInitFSTextureHelper();

CtsResult ctsDrawFSTexture(
    CtsDevice pDevice,
    CtsImage pImage
);

void ctsCleanupFSTextureHelper();

#ifdef __cplusplus
}
#endif