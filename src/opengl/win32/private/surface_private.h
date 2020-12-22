#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSurface {
    HINSTANCE instance;
    HWND window;
    HDC device;
    HGLRC context;
};

void ctsSurfaceMakeCurrent(CtsSurface pSurface);
void ctsSurfaceSwapBuffers(CtsSurface pSurface);

#ifdef __cplusplus
}
#endif

