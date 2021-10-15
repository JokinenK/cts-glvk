#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <stdbool.h>
#include <Windows.h>
#include <glad/glad.h>
#include "cts/gl_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsGlContext {
    HINSTANCE instance;
    HWND window;
    HDC device;
    HGLRC context;
    struct CtsGlHelper helper;
};

bool ctsInitGlContext(struct CtsGlContext* context, HWND window, HINSTANCE instance);
bool ctsInitGlContextOffscreen(struct CtsGlContext* context);
void ctsDestroyGlContext(struct CtsGlContext* context);
void ctsGlContextMakeCurrent(struct CtsGlContext* context);
void ctsGlContextClearCurrent(struct CtsGlContext* context);
void ctsGlContextSwapBuffers(struct CtsGlContext* context);

#ifdef __cplusplus
}
#endif