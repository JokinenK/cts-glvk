#pragma once

#include <cts/typedefs/device_size.h>
#include <cts/typedefs/extent.h>
#include <cts/typedefs/image_subresource_layers.h>
#include <cts/typedefs/offset.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBufferImageCopy {
    CtsDeviceSize bufferOffset;
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
    CtsImageSubresourceLayers imageSubresource;
    CtsOffset3D imageOffset;
    CtsExtent3D imageExtent;
} CtsBufferImageCopy;

#ifdef __cplusplus
}
#endif