#pragma once

#include <stdint.h>
#include <cts/typedefs/access_flags.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_size.h>
#include <cts/typedefs/image.h>
#include <cts/typedefs/image_subresource_range.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImageMemoryBarrier {
    CtsStructureType sType;
    const void* next;
    CtsAccessFlags srcAccessMask;
    CtsAccessFlags dstAccessMask;
    CtsImageLayout oldLayout;
    CtsImageLayout newLayout;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
    CtsImage image;
    CtsImageSubresourceRange subresourceRange;
} CtsImageMemoryBarrier;

#ifdef __cplusplus
}
#endif