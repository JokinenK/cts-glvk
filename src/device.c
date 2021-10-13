#include <stddef.h>
#include "vulkan/vk_icd.h"
#include <cts/device.h>
#include <private/private.h>
#include <private/instance_private.h>
#include <private/device_private.h>
#include <private/queue_private.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateDevice(
    VkPhysicalDevice physicalDeviceHandle,
    const VkDeviceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDevice* pDevice
) {
    struct CtsPhysicalDevice* physicalDevice = CtsPhysicalDeviceFromHandle(physicalDeviceHandle);
    struct CtsDevice* device = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDevice),
        alignof(struct CtsDevice),
        VK_SYSTEM_ALLOCATION_SCOPE_DEVICE
    );

    if (device == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(device);

    device->pName = "Testi";
    device->dynamicStateFlags = 0;
    device->physicalDevice = physicalDevice;
    device->activeGraphicsPipeline = NULL;
    device->queue = physicalDevice->queue;
    
    *pDevice = CtsDeviceToHandle(device);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsGetDeviceQueue(
    VkDevice deviceHandle,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex,
    VkQueue* pQueue
) {
    (void) queueFamilyIndex;
    (void) queueIndex;

    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    if (device != NULL) {
        *pQueue = CtsQueueToHandle(device->queue);
    }
}

void VKAPI_CALL ctsDestroyDevice(
    VkDevice deviceHandle,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    if (device != NULL) {
        ctsFree(pAllocator, device);
    }
}

#ifdef __cplusplus
}
#endif