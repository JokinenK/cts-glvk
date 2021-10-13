#include <stddef.h>
#include <Windows.h>
#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_win32.h"
#include <cts/instance.h>
#include <private/private.h>
#include <private/instance_private.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static struct VkExtensionProperties gKhrSurfaceExtension = {
    .extensionName = VK_KHR_SURFACE_EXTENSION_NAME,
    .specVersion = VK_KHR_SURFACE_SPEC_VERSION
};

static struct VkExtensionProperties gKhrWin32SurfaceExtension = {
    .extensionName = VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    .specVersion = VK_KHR_WIN32_SURFACE_SPEC_VERSION
};

VkResult VKAPI_CALL ctsCreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance* pInstance
) {
    struct CtsInstance* instance = ctsAllocation(
        pAllocator,
        sizeof(struct CtsInstance),
        alignof(struct CtsInstance),
        VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE
    ); 

    if (instance == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(instance);
    ctsPhysicalDeviceInit(&instance->physicalDevice, instance, pAllocator);
    *pInstance = CtsInstanceToHandle(instance);
   
    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsDestroyInstance(
    VkInstance instanceHandle,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsInstance* instance = CtsInstanceFromHandle(instanceHandle);

    if (instance != NULL) {
        ctsPhysicalDeviceDestroy(&instance->physicalDevice, pAllocator);
        ctsFree(pAllocator, instance);
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsEnumerateInstanceVersion(
    uint32_t* pApiVersion
) {
    *pApiVersion = VK_MAKE_VERSION(1, 0, 0);
    return VK_SUCCESS;
}

VkResult ctsEnumerateInstanceExtensionProperties(
    const char* pLayerName,
    uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties
) {
    if (pPropertyCount != NULL) {
        *pPropertyCount = 2;
    }

    if (pProperties != NULL) {
        pProperties[0] = gKhrSurfaceExtension;
        pProperties[1] = gKhrWin32SurfaceExtension;
    }

    return VK_SUCCESS;
}

VkResult ctsEnumerateInstanceLayerProperties(
    uint32_t* pPropertyCount,
    VkLayerProperties* pProperties
) {
    if (pPropertyCount != NULL) {
        *pPropertyCount = 0;
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsEnumeratePhysicalDevices(
    VkInstance instanceHandle,
    uint32_t* pPhysicalDeviceCount,
    VkPhysicalDevice* pPhysicalDevices
) {
    struct CtsInstance* instance = CtsInstanceFromHandle(instanceHandle);

    if (pPhysicalDeviceCount != NULL) {
        *pPhysicalDeviceCount = 1;
    }

    if (pPhysicalDevices != NULL) {
        *pPhysicalDevices = CtsPhysicalDeviceToHandle(&instance->physicalDevice);
    }

    return VK_SUCCESS;
}

#ifdef __cplusplus
}
#endif
