#pragma once

#include <cts/allocator.h>
#include <cts/types.h>
#include <cts/typedefs/win32_surface.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateWin32Surface(
    CtsInstance instance,
    const CtsWin32SurfaceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSurface* pSurface
);

void ctsDestroyWin32Surface(
    CtsSurface surface,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsGetPhysicalDeviceSurfaceSupport(
    CtsPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    CtsSurface surface,
    CtsBool32* pSupported
);

CtsResult ctsGetPhysicalDeviceSurfaceCapabilities(
    CtsPhysicalDevice physicalDevice,
    CtsSurface surface,
    CtsSurfaceCapabilities* pSurfaceCapabilities
);

CtsResult ctsGetPhysicalDeviceSurfaceFormats(
    CtsPhysicalDevice physicalDevice,
    CtsSurface surface,
    uint32_t* pSurfaceFormatCount,
    CtsSurfaceFormat* pSurfaceFormats
);

CtsResult ctsGetPhysicalDeviceSurfacePresentModes(
    CtsPhysicalDevice physicalDevice,
    CtsSurface surface,
    uint32_t* pPresentModeCount,
    CtsPresentMode* pPresentModes
);

#ifdef __cplusplus
}
#endif