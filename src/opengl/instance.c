#include <stddef.h>
#include <cts/instance.h>
#include <cts/mutex.h>
#include <cts/condition_variable.h>
#include <private/instance_private.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateInstance(
    const CtsInstanceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsInstance* pInstance
) {
    CtsInstance instance = ctsAllocation(
        pAllocator,
        sizeof(struct CtsInstanceImpl),
        alignof(struct CtsInstanceImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_INSTANCE
    ); 

    if (instance == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    ctsPhysicalDeviceInit(&instance->physicalDevice, instance, pAllocator);

    *pInstance = instance;
    return CTS_SUCCESS;
}

CtsResult ctsDestroyInstance(
    CtsInstance instance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (instance != NULL) {
        ctsPhysicalDeviceDestroy(&instance->physicalDevice, pAllocator);
        ctsFree(pAllocator, instance);
    }

    return CTS_SUCCESS;
}

CtsResult ctsEnumeratePhysicalDevices(
    CtsInstance instance,
    uint32_t* pPhysicalDeviceCount,
    CtsPhysicalDevice* pPhysicalDevices
) {
    if (pPhysicalDeviceCount != NULL) {
        *pPhysicalDeviceCount = 1;
    }

    if (pPhysicalDevices != NULL) {
        *pPhysicalDevices = &instance->physicalDevice;
    }

    return CTS_SUCCESS;
}

#ifdef __cplusplus
}
#endif
