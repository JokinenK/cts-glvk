#include <stddef.h>
#include <cts/instance.h>
#include <private/private.h>
#include <private/instance_private.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateInstance(
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

    ctsPhysicalDeviceInit(&instance->physicalDevice, instance, pAllocator);

    *pInstance = CtsInstanceToHandle(instance);
    return VK_SUCCESS;
}

VkResult ctsDestroyInstance(
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

VkResult ctsEnumeratePhysicalDevices(
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
