#pragma once

#include <windows.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/win32_surface_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsWin32SurfaceCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsWin32SurfaceCreateFlags flags;
    HINSTANCE hinstance;
    HWND hwnd;
} CtsWin32SurfaceCreateInfo;

#ifdef __cplusplus
}
#endif

