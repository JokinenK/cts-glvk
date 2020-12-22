#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_create_flags.h>
#include <cts/typedefs/device_queue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDevice* CtsDevice;
typedef struct CtsDeviceCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsDeviceCreateFlags flags;
    uint32_t pQueueCreateInfoCount;
    const CtsDeviceQueueCreateInfo* pQueueCreateInfos;
    /*uint32_t pEnabledLayerCount;
    const char* const* pEnabledLayerNames;
    uint32_t pEnabledExtensionCount;
    const char* const* pEnabledExtensionNames;
    const CtsPhysicalDeviceFeatures* pEnabledFeatures;*/
} CtsDeviceCreateInfo;

#ifdef __cplusplus
}
#endif