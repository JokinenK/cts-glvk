#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/image_aspect_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImageSubresourceLayers {
    CtsImageAspectFlags aspectMask;
    uint32_t            mipLevel;
    uint32_t            baseArrayLayer;
    uint32_t            layerCount;
} CtsImageSubresourceLayers;

#ifdef __cplusplus
}
#endif
