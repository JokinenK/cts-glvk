#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsImageAspectFlags;
typedef enum CtsImageAspectFlagBits {
    CTS_IMAGE_ASPECT_COLOR_BIT = 0x00000001,
    CTS_IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
    CTS_IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
    CTS_IMAGE_ASPECT_METADATA_BIT = 0x00000008
} CtsImageAspectFlagBits;

#ifdef __cplusplus
}
#endif