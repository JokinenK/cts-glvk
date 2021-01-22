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

    CtsPhysicalDevice physicalDevice = &instance->physicalDevice;
    physicalDevice->instance = instance;

    CtsMutexCreateInfo mutexCreateInfo;
    ctsCreateMutex(&mutexCreateInfo, pAllocator, &physicalDevice->mutex);

    CtsConditionVariableCreateInfo conditionVariableCreateInfo;
    ctsCreateConditionVariable(&conditionVariableCreateInfo, pAllocator, &physicalDevice->conditionVariable);

    *pInstance = instance;
    return CTS_SUCCESS;
}

CtsResult ctsDestroyInstance(
    CtsInstance instance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (instance != NULL) {
        ctsConditionVariableWakeAll(instance->physicalDevice.conditionVariable);
        ctsDestroyConditionVariable(instance->physicalDevice.conditionVariable, pAllocator);
        ctsDestroyMutex(instance->physicalDevice.mutex, pAllocator);
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
