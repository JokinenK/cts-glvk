#pragma once

#include <stdint.h>
#include <windows.h>
#include <cts/surface.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSurfaceImpl {
    HINSTANCE instance;
    HWND window;
    HDC device;
    HGLRC context;
    bool init;
};

void ctsSurfaceMakeCurrent(CtsSurface surface);
void ctsSurfaceSwapBuffers(CtsSurface surface);
bool ctsSurfaceQueryDeviceDetails(CtsSurface surface, uint32_t vendorId, uint32_t* pDeviceId, uint8_t* pUUID);
CtsResult ctsGetSurfaceExtent(CtsSurface surface, CtsExtent2D* pExtent);

#ifdef __cplusplus
}
#endif

