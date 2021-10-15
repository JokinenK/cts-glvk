#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <WinUser.h>
#include "glad/glad.h"
#include "cts/surface.h"
#include "cts/device_info.h"
#include "cts/commands.h"
#include "cts/private.h"
#include "cts/surface_private.h"
#include "cts/instance_private.h"
#include "cts/queue_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static VkSurfaceCapabilitiesKHR surfaceCapabilities = {
    .currentExtent = {1024, 1024},
    .currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    .minImageCount = 1,
    .maxImageCount = 2,
    .currentExtent = {0xFFFFFFFF, 0xFFFFFFFF},
    .minImageExtent = {1, 1},
    .maxImageExtent = {8192, 8192},
    .maxImageArrayLayers = 1,
    .supportedCompositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR,
    .supportedUsageFlags = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM,
    .supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
};

VkResult VKAPI_CALL ctsCreateWin32SurfaceKHR(
    VkInstance instanceHandle,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface
) {
    struct CtsInstance* instance = CtsInstanceFromHandle(instanceHandle);
    struct CtsPhysicalDevice* physicalDevice = &instance->physicalDevice;
    struct CtsSurface* surface = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSurface),
        alignof(struct CtsSurface),
        VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE
    );

    if (!surface) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(surface);
    if (!ctsInitGlContext(&surface->context, pCreateInfo->hwnd, pCreateInfo->hinstance)) {
        ctsDestroySurfaceKHR(instanceHandle, CtsSurfaceToHandle(surface), pAllocator);
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }

    ctsPhysicalDeviceSetSurface(physicalDevice, surface);
    *pSurface = CtsSurfaceToHandle(surface);

    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroySurfaceKHR(
    VkInstance instanceHandle,
    VkSurfaceKHR surfaceHandle,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsSurface* surface = CtsSurfaceFromHandle(surfaceHandle);

    ctsDestroyGlContext(&surface->context);
    ctsFree(pAllocator, surface);
}

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR surface,
    VkBool32* pSupported
) {
    *pSupported = true;
    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* pSurfaceCapabilities
) {
    *pSurfaceCapabilities = surfaceCapabilities;
    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormatKHR* pSurfaceFormats
) {
    if (pSurfaceFormatCount != NULL) {
        *pSurfaceFormatCount = 2;
    }

    if (pSurfaceFormats != NULL) {
        pSurfaceFormats[0].format = VK_FORMAT_B8G8R8A8_SRGB;
        pSurfaceFormats[0].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        pSurfaceFormats[1].format = VK_FORMAT_B8G8R8A8_UNORM;
        pSurfaceFormats[1].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pPresentModeCount,
    VkPresentModeKHR* pPresentModes
) {
    if (pPresentModeCount != NULL) {
        *pPresentModeCount = 1;
    }

    if (pPresentModes != NULL) {
        *pPresentModes = VK_PRESENT_MODE_FIFO_KHR;
    }

    return VK_SUCCESS;
}

VkBool32 ctsGetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex
) {
    return VK_TRUE;
}

struct CtsGlContext* ctsSurfaceGetGlContext(struct CtsSurface* surface)
{
    return &surface->context;
}

bool ctsSurfaceQueryDeviceDetails(struct CtsSurface* surface, uint32_t vendorId, uint32_t* pDeviceId, uint8_t* pUUID)
{
    CtsWin32DeviceInfo deviceInfo;
    if (ctsWin32ParseDeviceInfo(vendorId, &deviceInfo)) {
        *pDeviceId = deviceInfo.deviceId;
        memcpy(pUUID, deviceInfo.uuid, sizeof(deviceInfo.uuid));

        return true;
    }

    return false;
}

VkResult ctsGetSurfaceExtent(struct CtsSurface* surface, VkExtent2D* pExtent)
{
    RECT rect;
    if (GetWindowRect(surface->context.window, &rect)) {
        pExtent->width = rect.right - rect.left;
        pExtent->height = rect.bottom - rect.top;
        return VK_SUCCESS;
    }

    return VK_ERROR_INITIALIZATION_FAILED;
}

#ifdef __cplusplus
}
#endif