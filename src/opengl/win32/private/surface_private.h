#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <stdint.h>
#include "cts/surface.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSurface {
    HINSTANCE instance;
    HWND window;
    HDC device;
    HGLRC context;
    bool init;
};

void ctsSurfaceMakeCurrent(struct CtsSurface* surface);
void ctsSurfaceSwapBuffers(struct CtsSurface* surface);
bool ctsSurfaceQueryDeviceDetails(struct CtsSurface* surface, uint32_t vendorId, uint32_t* pDeviceId, uint8_t* pUUID);
VkResult ctsGetSurfaceExtent(struct CtsSurface* surface, VkExtent2D* pExtent);

#ifdef __cplusplus
}
#endif

