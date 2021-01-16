#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/extent.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSurfaceCapabilities {
    uint32_t minImageCount;
    uint32_t maxImageCount;
    CtsExtent2D currentExtent;
    CtsExtent2D minImageExtent;
    CtsExtent2D maxImageExtent;
    uint32_t maxImageArrayLayers;
    /*CtsSurfaceTransformFlags supportedTransforms;
    CtsSurfaceTransformFlagBits currentTransform;
    CtsCompositeAlphaFlags supportedCompositeAlpha;
    CtsImageUsageFlags supportedUsageFlags;*/
} CtsSurfaceCapabilities;

#ifdef __cplusplus
}
#endif


