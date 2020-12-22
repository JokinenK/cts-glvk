#pragma once

#include <stdint.h>
#include <cts/typedefs/extent.h>
#include <cts/typedefs/image_subresource_layers.h>
#include <cts/typedefs/offset.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImageResolve {
    CtsImageSubresourceLayers srcSubresource;
    CtsOffset3D srcOffset;
    CtsImageSubresourceLayers dstSubresource;
    CtsOffset3D dstOffset;
    CtsExtent3D extent;
} CtsImageResolve;

#ifdef __cplusplus
}
#endif