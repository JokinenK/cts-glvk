#pragma once

#include <cts/typedefs/offset.h>
#include <cts/typedefs/image_subresource_layers.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImageBlit {
    CtsImageSubresourceLayers srcSubresource;
    CtsOffset3D srcOffsets[2];
    CtsImageSubresourceLayers dstSubresource;
    CtsOffset3D dstOffsets[2];
} CtsImageBlit;

#ifdef __cplusplus
}
#endif
