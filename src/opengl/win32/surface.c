#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <cts/surface.h>
#include <cts/device_info.h>
#include <cts/typedefs/bool.h>
#include <cts/typedefs/physical_device.h>
#include <private/surface_private.h>
#include <private/instance_private.h>
#include <private/queue_private.h>

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

static PFN_wglCreateContextAttribsARB* wglCreateContextAttribsARB = NULL;
static PFN_wglChoosePixelFormatARB* wglChoosePixelFormatARB = NULL;
static bool gladLoaded = false;

static CtsSurfaceCapabilities surfaceCapabilities = {
    .minImageCount = 1,
    .maxImageCount = 2,
    .currentExtent = {0xFFFFFFFF, 0xFFFFFFFF},
    .minImageExtent = {1, 1},
    .maxImageExtent = {8192, 8192},
    .maxImageArrayLayers = 1
};

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
static bool initOpenGLExtensions();

CtsResult ctsCreateWin32Surface(
    CtsInstance instance,
    const CtsWin32SurfaceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSurface* pSurface
) {
    if (!initOpenGLExtensions()) {
        return CTS_NOT_READY;
    }

    CtsSurface surface = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSurfaceImpl),
        alignof(struct CtsSurfaceImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_INSTANCE
    );

    if (surface) {
        /**
         * TODO: Apply some sort of hinting
         * 
         * glfwWindowHint(GLFW_RED_BITS, 8);
         * glfwWindowHint(GLFW_GREEN_BITS, 8);
         * glfwWindowHint(GLFW_BLUE_BITS, 8);
         * glfwWindowHint(GLFW_ALPHA_BITS, 8);
         * 
         * glfwWindowHint(GLFW_DEPTH_BITS, 24);
         * glfwWindowHint(GLFW_STENCIL_BITS, 8);
         * 
         * glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
         */

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

        HDC device = GetDC(pCreateInfo->hwnd);
        int pixelFormat;
        UINT numFormats;
        wglChoosePixelFormatARB(device, pixelFormatAttribs, 0, 1, &pixelFormat, &numFormats);

        if (!numFormats) {
            fprintf(stderr, "Failed to set the OpenGL 3.3 pixel format.");
            return CTS_NOT_READY;
        }

        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(device, pixelFormat, sizeof(pfd), &pfd);
        
        if (!SetPixelFormat(device, pixelFormat, &pfd)) {
            fprintf(stderr, "Failed to set the OpenGL 3.3 pixel format.");
            return CTS_NOT_READY;
        }

        // Specify that we want to create an OpenGL 3.3 core profile context
        int gl33Attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };

        HGLRC gl33Context = wglCreateContextAttribsARB(device, 0, gl33Attribs);
        if (!gl33Context) {
            fprintf(stderr, "Failed to create OpenGL 3.2 context.");
            return CTS_NOT_READY;
        }

        surface->device = device;
        surface->window = pCreateInfo->hwnd;
        surface->instance = pCreateInfo->hinstance;
        surface->device = device;
        surface->context = gl33Context;

        instance->physicalDevice.surface = surface;
        ctsConditionVariableWakeAll(instance->physicalDevice.conditionVariable);

        *pSurface = surface;
        return CTS_SUCCESS;
    }

    return CTS_NOT_READY;
}

void ctsDestroyWin32Surface(
    CtsSurface surface,
    const CtsAllocationCallbacks* pAllocator
) {
    if (surface) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(surface->context);
        ctsFree(pAllocator, surface);
    }
}

CtsResult ctsGetPhysicalDeviceSurfaceSupport(
    CtsPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    CtsSurface surface,
    CtsBool32* pSupported
) {
    *pSupported = true;
    return CTS_SUCCESS;
}

CtsResult ctsGetPhysicalDeviceSurfaceCapabilities(
    CtsPhysicalDevice physicalDevice,
    CtsSurface surface,
    CtsSurfaceCapabilities* pSurfaceCapabilities
) {
    *pSurfaceCapabilities = surfaceCapabilities;
    return CTS_SUCCESS;
}

CtsResult ctsGetPhysicalDeviceSurfaceFormats(
    CtsPhysicalDevice physicalDevice,
    CtsSurface surface,
    uint32_t* pSurfaceFormatCount,
    CtsSurfaceFormat* pSurfaceFormats
) {
    if (pSurfaceFormatCount != NULL) {
        *pSurfaceFormatCount = 2;
    }

    if (pSurfaceFormats != NULL) {
        pSurfaceFormats[0].format = CTS_FORMAT_B8G8R8A8_SRGB;
        pSurfaceFormats[0].colorSpace = CTS_COLOR_SPACE_SRGB_NONLINEAR;

        pSurfaceFormats[1].format = CTS_FORMAT_B8G8R8A8_UNORM;
        pSurfaceFormats[1].colorSpace = CTS_COLOR_SPACE_SRGB_NONLINEAR;
    }

    return CTS_SUCCESS;
}

CtsResult ctsGetPhysicalDeviceSurfacePresentModes(
    CtsPhysicalDevice physicalDevice,
    CtsSurface surface,
    uint32_t* pPresentModeCount,
    CtsPresentMode* pPresentModes
) {
    if (pPresentModeCount != NULL) {
        *pPresentModeCount = 1;
    }

    if (pPresentModes != NULL) {
        *pPresentModes = CTS_PRESENT_MODE_FIFO;
    }

    return CTS_SUCCESS;
}

bool ctsSurfaceQueryDeviceDetails(CtsSurface surface, uint32_t vendorId, uint32_t* pDeviceId, uint8_t* pUUID)
{
    CtsWin32DeviceInfo deviceInfo;
    if (ctsWin32ParseDeviceInfo(vendorId, &deviceInfo)) {
        *pDeviceId = deviceInfo.deviceId;
        memcpy(pUUID, deviceInfo.uuid, sizeof(deviceInfo.uuid));

        return true;
    }

    return false;
}

void ctsSurfaceMakeCurrent(CtsSurface surface)
{
    if (surface->device && surface->context) {
        wglMakeCurrent(surface->device, surface->context);
        initGlad();
    }
}

void ctsSurfaceSwapBuffers(CtsSurface surface)
{
    SwapBuffers(surface->device);
}

CtsResult ctsGetSurfaceExtent(CtsSurface surface, CtsExtent2D* pExtent)
{
    RECT rect;
    if (GetWindowRect(surface->window, &rect)) {
        pExtent->width = rect.right - rect.left;
        pExtent->height = rect.bottom - rect.top;
        return CTS_SUCCESS;
    }

    return CTS_ERROR_UNKNOWN;
}

static bool initOpenGLExtensions() 
{
    if (wglChoosePixelFormatARB != NULL && wglCreateContextAttribsARB != NULL) {
        return true;
    }

    // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
    // We use a dummy window because you can only set the pixel format for a window once. For the
    // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
    // have a context.
    WNDCLASSA windowClass = {
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = DefWindowProcA,
        .hInstance = GetModuleHandle(0),
        .lpszClassName = "Dummy_WGL",
    };

    if (!RegisterClassA(&windowClass)) {
        fprintf(stderr, "Failed to register dummy OpenGL window.");
        return false;
    }

    HWND dummyWindow = CreateWindowExA(
        0,
        windowClass.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        windowClass.hInstance,
        0
    );

    if (!dummyWindow) {
        fprintf(stderr, "Failed to create dummy OpenGL window.");
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

    wglMakeCurrent(dummyDc, 0);
    wglDeleteContext(dummyContext);
    ReleaseDC(dummyWindow, dummyDc);
    DestroyWindow(dummyWindow);

    return true;
}

static bool initGlad()
{
    if (gladLoaded) {
        return true;
    }

    gladLoaded = (gladLoadGL() == 1);
    return gladLoaded;
}

#ifdef __cplusplus
}
#endif