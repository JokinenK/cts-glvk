#pragma once

#include <cts/allocator.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/instance.h>
#include <cts/typedefs/surface.h>
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

CtsResult ctsDestroyWin32Surface(
    CtsSurface surface,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif