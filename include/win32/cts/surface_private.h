#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <stdint.h>
#include "cts/surface.h"
#include "cts/fullscreen_texture.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSurface {
    struct CtsObjectBase base;
    HINSTANCE instance;
    HWND window;
    HDC device;
    HGLRC context;
    struct CtsSurfaceHelper helper;
};

void ctsSurfaceMakeCurrent(struct CtsSurface* surface);
void ctsSurfaceClearCurrent(struct CtsSurface* surface);
void ctsSurfaceSwapBuffers(struct CtsSurface* surface);
bool ctsSurfaceQueryDeviceDetails(struct CtsSurface* surface, uint32_t vendorId, uint32_t* pDeviceId, uint8_t* pUUID);
VkResult ctsGetSurfaceExtent(struct CtsSurface* surface, VkExtent2D* pExtent);

bool ctsInitOffscreenSurface(struct CtsSurface* result);
void ctsDestroyOffscreenSurface(struct CtsSurface* surface);

#ifdef __cplusplus
}
#endif

