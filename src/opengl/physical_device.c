#include <stddef.h>
#include <cts/instance.h>
#include <cts/mutex.h>
#include <cts/condition_variable.h>
#include <private/instance_private.h>
#include <private/physical_device_private.h>
#include <private/swapchain_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsGetPhysicalDeviceQueueFamilyProperties(
    CtsPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    CtsQueueFamilyProperties* pQueueFamilyProperties
) {
    if (pQueueFamilyPropertyCount != NULL) {
        *pQueueFamilyPropertyCount = 1;
    }

    if (pQueueFamilyProperties != NULL) {
        *pQueueFamilyProperties = gQueueFamilyProperties;
    }

    return CTS_SUCCESS;
}

CtsResult ctsEnumerateDeviceExtensionProperties(
    CtsPhysicalDevice physicalDevice,
    const char* pLayerName,
    uint32_t* pPropertyCount,
    CtsExtensionProperties* pProperties
) {
    static const CtsExtensionProperties* properties[] = {
        &swapchainExtensionProperties,
    };

    static uint32_t propertyCount = CTS_ARRAY_SIZE(properties);

    if (pPropertyCount != NULL) {
        *pPropertyCount = propertyCount;
    }

    if (pProperties != NULL) {
        for (uint32_t i = 0; i < propertyCount; ++i) {
            pProperties[i] = *properties[i];
        }
    }

    return CTS_SUCCESS;
}

void ctsGetPhysicalDeviceMemoryProperties(
    CtsPhysicalDevice physicalDevice,
    CtsPhysicalDeviceMemoryProperties* pMemoryProperties
) {
    if (pMemoryProperties != NULL) {
        *pMemoryProperties = gPhysicalMemoryProperties;
    }
}

const CtsMemoryType* ctsGetMemoryType(uint32_t memoryTypeIndex)
{
    return &gPhysicalMemoryProperties.memoryTypes[memoryTypeIndex];
}

#ifdef __cplusplus
}
#endif
