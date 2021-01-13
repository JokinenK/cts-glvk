#pragma once

#include <cts/typedefs/bool.h>
#include <cts/typedefs/semaphore.h>
#include <cts/typedefs/surface.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/extent.h>
#include <cts/typedefs/image_usage_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsSwapchainCreateFlags;

typedef struct CtsSwapchain* CtsSwapchain;
typedef struct CtsSwapchainCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsSwapchainCreateFlags flags;
    CtsSurface surface;
    uint32_t minImageCount;
    CtsFormat imageFormat;
    //CtsColorSpace imageColorSpace;
    CtsExtent2D imageExtent;
    uint32_t imageArrayLayers;
    CtsImageUsageFlags imageUsage;
    //CtsSharingMode imageSharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t* pQueueFamilyIndices;
    //CtsSurfaceTransformFlagBitsKHR preTransform;
    //CtsCompositeAlphaFlagBitsKHR compositeAlpha;
    //CtsPresentModeKHR presentMode;
    CtsBool32 clipped;
    CtsSwapchain oldSwapchain;
} CtsSwapchainCreateInfo;

#ifdef __cplusplus
}
#endif
