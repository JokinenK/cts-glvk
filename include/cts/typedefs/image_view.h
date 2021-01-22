#pragma once

#include <stdint.h>
#include <cts/typedefs/component_mapping.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/extent.h>
#include <cts/typedefs/image.h>
#include <cts/typedefs/image_subresource_range.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImageViewImpl* CtsImageView;

typedef struct CtsImageViewCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsImage image;
    CtsImageViewType viewType;
    CtsFormat format;
    CtsComponentMapping components;
    CtsImageSubresourceRange subresourceRange;
} CtsImageViewCreateInfo;

#ifdef __cplusplus
}
#endif