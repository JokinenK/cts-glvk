#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsInitFSTextureHelper();

CtsResult ctsDrawFSTexture(
    CtsDevice device,
    CtsImage image
);

void ctsCleanupFSTextureHelper();

#ifdef __cplusplus
}
#endif