#include <assert.h>
#include <stdio.h>
#include "cts/gl_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef HGLRC WINAPI PFN_wglCreateContextAttribsARB(
    HDC hdc, 
    HGLRC hShareContext, 
    const int *attribList
);

typedef BOOL WINAPI PFN_wglChoosePixelFormatARB(
    HDC hdc, 
    const int *piAttribIList,
    const FLOAT *pfAttribFList, 
    UINT nMaxFormats, 
    int *piFormats, 
    UINT *nNumFormats
);

static bool gGladLoaded = false;
static PFN_wglCreateContextAttribsARB* wglCreateContextAttribsARB = NULL;
static PFN_wglChoosePixelFormatARB* wglChoosePixelFormatARB = NULL;

// See https://www.opengl.org/registry/specs/ARB/wgl_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

// See https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023
#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

static bool initGlad();
static bool initExtensions();
static HWND createWindow(const char* className, const char* title);

bool ctsInitGlWindow(HWND window, HDC dc)
{
    if (!initExtensions()) {
        return false;
    }

    int pixelFormatAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    int pixelFormat;
    UINT numFormats;
    wglChoosePixelFormatARB(dc, pixelFormatAttribs, 0, 1, &pixelFormat, &numFormats);

    if (!numFormats) {
        fprintf(stderr, "Failed to query the pixel formats.");
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd);
    
    if (!SetPixelFormat(dc, pixelFormat, &pfd)) {
        fprintf(stderr, "Failed to set the pixel format.");
        return false;
    }

    return true;
}

bool ctsInitGlContext(struct CtsGlContext* context)
{
    if (!initExtensions()) {
        return false;
    }

    HWND window = createWindow("ctsRenderer", "OffscreenWindow");
    HINSTANCE instance = GetModuleHandle(NULL);
    HDC dc = GetDC(window);
    HDC prevDc = wglGetCurrentDC();
    HGLRC prevCtx = wglGetCurrentContext();

    if (!ctsInitGlWindow(window, dc)) {
        return false;
    }

    // Specify that we want to create an OpenGL 3.3 core profile context
    int attributes[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    wglMakeCurrent(prevDc, NULL);
    
    HGLRC ctx = wglCreateContextAttribsARB(dc, NULL, attributes);

    if (!ctx) {
        return false;
    }

    wglMakeCurrent(dc, ctx);
    initGlad();
    ctsInitGlHelper();
    
    wglMakeCurrent(dc, NULL);
    wglMakeCurrent(prevDc, prevCtx);

    context->window = window;
    context->instance = instance;
    context->device = dc;
    context->context = ctx;

    return true;
}

void ctsDestroyGlContext(struct CtsGlContext* context)
{
    wglMakeCurrent(context->device, context->context);
    ctsDestroyGlHelper();
    wglMakeCurrent(context->device, NULL);
    wglDeleteContext(context->context);
    ReleaseDC(context->window, context->device);
    DestroyWindow(context->window);
}

void ctsGlContextActivate(struct CtsGlContext* context)
{
    if (!wglMakeCurrent(context->device, context->context)) {
        fprintf(stderr, "ctsGlContextActivate failed: %d\n", GetLastError());
    }
}

void ctsGlContextDeactivate(struct CtsGlContext* context)
{
    if (!wglMakeCurrent(context->device, NULL)) {
        fprintf(stderr, "ctsGlContextDeactivate failed: %d\n", GetLastError());
    }
}

void ctsGlContextSwapBuffers(struct CtsGlContext* context)
{
    if (!SwapBuffers(context->device)) {
        fprintf(stderr, "ctsGlContextSwapBuffers failed: %d\n", GetLastError());
    }
}


static bool initGlad()
{
    if (!gGladLoaded) {
        gGladLoaded = (gladLoadGL() == 1);
    }
    
    return gGladLoaded;
}

static bool initExtensions() 
{
    if (wglChoosePixelFormatARB != NULL && wglCreateContextAttribsARB != NULL) {
        return true;
    }

    // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
    // We use a dummy window because you can only set the pixel format for a window once. For the
    // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
    // have a context.

    HWND dummyWindow = createWindow("ctsRendererDummy", "DummyContextWindow");
    if (!dummyWindow) {
        return false;
    }

    HDC dummyDc = GetDC(dummyWindow);

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .iPixelType = PFD_TYPE_RGBA,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .cColorBits = 32,
        .cAlphaBits = 8,
        .iLayerType = PFD_MAIN_PLANE,
        .cDepthBits = 24,
        .cStencilBits = 8,
    };

    int pixelFormat = ChoosePixelFormat(dummyDc, &pfd);
    if (!pixelFormat) {
        fprintf(stderr, "Failed to find a suitable pixel format.");
        return false;
    }

    if (!SetPixelFormat(dummyDc, pixelFormat, &pfd)) {
        fprintf(stderr, "Failed to set the pixel format.");
        return false;
    }

    HGLRC dummyContext = wglCreateContext(dummyDc);
    if (!dummyContext) {
        fprintf(stderr, "Failed to create a dummy OpenGL rendering context.");
        return false;
    }

    if (!wglMakeCurrent(dummyDc, dummyContext)) {
        fprintf(stderr, "Failed to activate dummy OpenGL rendering context.");
        return false;
    }

    wglCreateContextAttribsARB = (PFN_wglCreateContextAttribsARB*) wglGetProcAddress(
        "wglCreateContextAttribsARB"
    );

    wglChoosePixelFormatARB = (PFN_wglChoosePixelFormatARB*) wglGetProcAddress(
        "wglChoosePixelFormatARB"
    );

    wglMakeCurrent(dummyDc, NULL);
    wglDeleteContext(dummyContext);
    ReleaseDC(dummyWindow, dummyDc);
    DestroyWindow(dummyWindow);

    return true;
}

static HWND createWindow(const char* className, const char* title)
{
    WNDCLASS wnd;
    memset(&wnd, 0, sizeof(wnd));
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hCursor = LoadCursor(0, IDC_ARROW);
    wnd.hIcon = LoadIcon(0, IDI_WINLOGO);
    wnd.lpszMenuName = 0;
    wnd.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wnd.hbrBackground = 0;
    wnd.lpfnWndProc = DefWindowProcA;
    wnd.hInstance = GetModuleHandle(NULL);
    wnd.lpszClassName = className;

    if (!RegisterClass(&wnd)) {
        fprintf(stderr, "Failed to register window.");
        return NULL;
    }

    HWND window = CreateWindowEx(
        0,
        className,
        title,

        // Window style
        WS_OVERLAPPEDWINDOW,
        
        // Size and position
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,

        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!window) {
        fprintf(stderr, "Failed to create window.");
        return NULL;
    }

    return window;
}

#ifdef __cplusplus
}
#endif