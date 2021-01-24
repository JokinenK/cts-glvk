#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/extent.h>
#include <cts/typedefs/image_create_flags.h>
#include <cts/typedefs/image_usage_flags.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImageImpl* CtsImage;

typedef struct CtsImageCreateInfo {
    CtsStructureType        sType;
    const void*             pNext;
    CtsImageCreateFlags     flags;
    CtsImageType            imageType;
    CtsFormat               format;
    CtsExtent3D             extent;
    uint32_t                mipLevels;
    uint32_t                arrayLayers;
    CtsSampleCountFlagBits  samples;
    CtsImageTiling          tiling;
    CtsImageUsageFlags      usage;
    CtsSharingMode          sharingMode;
    uint32_t                queueFamilyIndexCount;
    const uint32_t*         pQueueFamilyIndices;
    CtsImageLayout          initialLayout;
} CtsImageCreateInfo;

#ifdef __cplusplus
}
#endif