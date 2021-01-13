#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/extent.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsImage* CtsImage;

typedef struct CtsImageCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsImageType imageType;
    CtsFormat imageFormat;
    CtsExtent3D extent;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    CtsSampleCountFlagBits samples;
    //VkImageTiling tiling;
    CtsImageUsage usage;
    //VkSharingMode sharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t* pQueueFamilyIndices;
    //VkImageLayout initialLayout;
} CtsImageCreateInfo;

#ifdef __cplusplus
}
#endif