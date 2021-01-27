#include <stddef.h>
#include <cts/device.h>
#include <private/device_private.h>
#include <private/queue_private.h>
#include <private/physical_device_private.h>

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
        sizeof(struct CtsDeviceImpl),
        alignof(struct CtsDeviceImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_DEVICE
    );

    if (device == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    device->dynamicStateFlags = 0;
    device->physicalDevice = physicalDevice;
    device->activeGraphicsPipeline = NULL;
    device->queue = physicalDevice->queue;
    
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
        ctsFree(pAllocator, device);
    }
}

#ifdef __cplusplus
}
#endif