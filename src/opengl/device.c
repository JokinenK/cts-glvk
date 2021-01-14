#include <stddef.h>
#include <cts/device.h>
#include <private/device_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDevice(
    CtsPhysicalDevice physicalDevice,
    const CtsDeviceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDevice* pDevice
) {
    CtsDevice device = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDevice),
        alignof(struct CtsDevice),
        CTS_SYSTEM_ALLOCATION_SCOPE_DEVICE
    );

    if (device == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    device->isRunning = true;
    device->surface = NULL;
    device->activeDynamicState = 0;
    device->activeIndexBuffer = NULL;
    device->activeVertexInputState = NULL;
    device->activeInputAssemblyState = NULL;
    device->activeTessellationState = NULL;
    device->activeViewportState = NULL;
    device->activeRasterizationState = NULL;
    device->activeMultisampleState = NULL;
    device->activeDepthStencilState = NULL;
    device->activeColorBlendState = NULL;
    device->physicalDevice = physicalDevice;

    CtsQueueCreateInfo queueCreateInfo;
    queueCreateInfo.device = device;
    queueCreateInfo.size = 32;
    ctsCreateQueue(&queueCreateInfo, pAllocator, &device->queue);

    CtsMutexCreateInfo mutexCreateInfo;
    ctsCreateMutex(&mutexCreateInfo, pAllocator, &device->init.mutex);
    ctsCreateMutex(&mutexCreateInfo, pAllocator, &device->dispatch.mutex);

    CtsConditionVariableCreateInfo conditionVariableCreateInfo;
    ctsCreateConditionVariable(&conditionVariableCreateInfo, pAllocator, &device->init.conditionVariable);
    ctsCreateConditionVariable(&conditionVariableCreateInfo, pAllocator, &device->dispatch.conditionVariable);

    *pDevice = device;
    return CTS_SUCCESS;
}

void ctsGetDeviceQueue(
    CtsDevice device,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex,
    CtsQueue* pQueue
) {
    (void) queueFamilyIndex;
    (void) queueIndex;

    if (device != NULL) {
        *pQueue = device->queue;
    }
}

void ctsDestroyDevice(
    CtsDevice device,
    const CtsAllocationCallbacks* pAllocator
) {
    if (device != NULL) {
        device->isRunning = false;

        ctsConditionVariableWakeAll(device->init.conditionVariable);
        ctsConditionVariableWakeAll(device->dispatch.conditionVariable);

        ctsDestroyConditionVariable(device->init.conditionVariable, pAllocator);
        ctsDestroyConditionVariable(device->dispatch.conditionVariable, pAllocator);

        ctsDestroyMutex(device->init.mutex, pAllocator);
        ctsDestroyMutex(device->dispatch.mutex, pAllocator);

        ctsDestroyQueue(device->queue, pAllocator);
        ctsFree(pAllocator, device);
    }
}

#ifdef __cplusplus
}
#endif