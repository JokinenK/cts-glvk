#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_queue_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDeviceQueueCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsDeviceQueueCreateFlags flags;
    uint32_t queueFamilyIndex;
    uint32_t queueCount;
    const float* queuePriorities;
} CtsDeviceQueueCreateInfo;

#ifdef __cplusplus
}
#endif