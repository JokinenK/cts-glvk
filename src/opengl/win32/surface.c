#pragma once

#include <stdbool.h>
#include <cts/surface.h>
#include <cts/typedefs/bool.h>
#include <cts/typedefs/physical_device.h>
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

CtsResult ctsGetPhysicalDeviceSurfaceSupport(
    CtsPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    CtsSurface surface,
    CtsBool32* pSupported
) {
    *pSupported = true;
}

CtsResult ctsGetPhysicalDeviceSurfaceCapabilities(
    CtsPhysicalDevice physicalDevice,
    CtsSurface surface,
    CtsSurfaceCapabilities* pSurfaceCapabilities
) {
    pSurfaceCapabilities->minImageCount = 1;
    pSurfaceCapabilities->maxImageCount = 2;
    pSurfaceCapabilities->currentExtent = (CtsExtent2D){0xFFFFFFFF, 0xFFFFFFFF};
    pSurfaceCapabilities->minImageExtent = (CtsExtent2D){1, 1};
    pSurfaceCapabilities->maxImageExtent = (CtsExtent2D){8192, 8192};
    pSurfaceCapabilities->maxImageArrayLayers = 1;
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
        pSurfaceFormats[0].format = CTS_FORMAT_B8G8R8A8_UNORM;
        pSurfaceFormats[0].colorSpace = CTS_COLOR_SPACE_SRGB_NONLINEAR;

        pSurfaceFormats[1].format = CTS_FORMAT_B8G8R8A8_SRGB;
        pSurfaceFormats[1].colorSpace = CTS_COLOR_SPACE_SRGB_NONLINEAR;
    }
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