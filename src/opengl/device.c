#include <stddef.h>
#include <cts/device.h>
#include <private/device_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDevice(
    CtsPhysicalDevice pPhysicalDevice,
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
    device->physicalDevice = pPhysicalDevice;

    CtsQueueCreateInfo queueCreateInfo;
    queueCreateInfo.device = device;
    queueCreateInfo.size = 32;
    ctsCreateQueue(&queueCreateInfo, pAllocator, 1, &device->queue);

    CtsSemaphoreCreateInfo semaphoreCreateInfo;
    ctsCreateSemaphore(device, &semaphoreCreateInfo, pAllocator, &device->initSemaphore);
    ctsCreateSemaphore(device, &semaphoreCreateInfo, pAllocator, &device->dispatchSemaphore);

    *pDevice = device;
    return CTS_SUCCESS;
}

void ctsGetDeviceQueue(
    CtsDevice pDevice,
    uint32_t pQueueFamilyIndex,
    uint32_t pQueueIndex,
    CtsQueue* pQueue
) {
    (void) pQueueFamilyIndex;
    (void) pQueueIndex;

    if (pDevice != NULL) {
        *pQueue = pDevice->queue;
    }
}

void ctsDestroyDevice(
    CtsDevice pDevice,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pDevice) {
        pDevice->isRunning = false;

        ctsSignalSemaphores(1, &pDevice->initSemaphore);
        ctsSignalSemaphores(1, &pDevice->dispatchSemaphore);

        ctsDestroySemaphore(pDevice, pDevice->initSemaphore, pAllocator);
        ctsDestroySemaphore(pDevice, pDevice->dispatchSemaphore, pAllocator);
        ctsDestroyQueue(pDevice->queue, pAllocator);

        ctsFree(pAllocator, pDevice);
    }
}

#ifdef __cplusplus
}
#endif