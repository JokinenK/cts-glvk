#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/extent.h>
#include <cts/typedefs/image_aspect_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImageSubresourceRange {
    CtsImageAspectFlags aspectMask;
    uint32_t baseMipLevel;
    uint32_t levelCount;
    uint32_t baseArrayLayer;
    uint32_t layerCount;
} CtsImageSubresourceRange;

#ifdef __cplusplus
}
#endif