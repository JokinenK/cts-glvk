#pragma once

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
CtsResult ctsGetSurfaceExtent(CtsSurface surface, CtsExtent2D* pExtent);

#ifdef __cplusplus
}
#endif

