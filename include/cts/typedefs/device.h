#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_create_flags.h>
#include <cts/typedefs/device_queue.h>
#include <cts/typedefs/physical_device_features.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDevice* CtsDevice;
typedef struct CtsDeviceCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsDeviceCreateFlags flags;
    uint32_t queueCreateInfoCount;
    const CtsDeviceQueueCreateInfo* pQueueCreateInfos;
    uint32_t enabledLayerCount;
    const char* const* pEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* pEnabledExtensionNames;
    const CtsPhysicalDeviceFeatures* pEnabledFeatures;
} CtsDeviceCreateInfo;

#ifdef __cplusplus
}
#endif