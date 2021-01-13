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

void ctsSurfaceMakeCurrent(CtsSurface surface);
void ctsSurfaceSwapBuffers(CtsSurface surface);

#ifdef __cplusplus
}
#endif

