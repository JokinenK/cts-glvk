#pragma once

#include <stdbool.h>
#include <cts/surface_handler.h>
#include <private/surface_private.h>
#include <private/instance_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateWin32Surface(
    CtsInstance instance,
    const CtsWin32SurfaceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSurface* pSurface
) {
    CtsSurface surface = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSurface),
        alignof(struct CtsSurface),
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

        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            //Flags
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            // The kind of framebuffer. RGBA or palette.
            PFD_TYPE_RGBA,
            // Colordepth of the framebuffer.
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            // Number of bits for the depthbuffer
            24,
            // Number of bits for the stencilbuffer
            8,
            // Number of Aux buffers in the framebuffer.
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        HDC device = GetDC(pCreateInfo->hwnd);

        int pixelFormat = ChoosePixelFormat(device, &pfd); 
        SetPixelFormat(device, pixelFormat, &pfd);

        surface->window = pCreateInfo->hwnd;
        surface->instance = pCreateInfo->hinstance;
        surface->device = device;
        surface->context = wglCreateContext(surface->device);

        bool initialized = (gladLoadGL() == 1);
        assert(initialized && "Unable to load OpenGL");

        instance->surface = surface;
        *pSurface = surface;
        return CTS_SUCCESS;
    }

    return CTS_NOT_READY;
}

CtsResult ctsDestroyWin32Surface(
    CtsSurface surface,
    const CtsAllocationCallbacks* pAllocator
) {
    if (surface) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(surface->context);
        ctsFree(pAllocator, surface);
    }
}

void ctsSurfaceMakeCurrent(CtsSurface surface)
{
    if (surface->device && surface->context) {
        wglMakeCurrent(surface->device, surface->context);
    }
}

void ctsSurfaceSwapBuffers(CtsSurface surface)
{
    SwapBuffers(surface->device);
}

#ifdef __cplusplus
}
#endif