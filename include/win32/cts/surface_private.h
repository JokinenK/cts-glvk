#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <stdint.h>
#include "cts/surface.h"
#include "cts/object_base.h"
#include "cts/gl_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSurface {
    struct CtsObjectBase base;
    HINSTANCE instance;
    HWND window;
    HDC device;
};

VkResult ctsCreateWin32SurfaceKHRImpl(
    VkInstance instanceHandle,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface
);

void ctsDestroySurfaceKHRImpl(
    VkInstance instanceHandle,
    VkSurfaceKHR surfaceHandle,
    const VkAllocationCallbacks* pAllocator
);

VkResult ctsGetSurfaceExtent(struct CtsSurface* surface, VkExtent2D* pExtent);

#ifdef __cplusplus
}
#endif

