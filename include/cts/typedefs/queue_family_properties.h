#pragma once

#include <stdint.h>
#include <cts/typedefs/extent.h>
#include <cts/typedefs/queue_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueueFamilyProperties {
    CtsQueueFlags   queueFlags;
    uint32_t        queueCount;
    uint32_t        timestampValidBits;
    CtsExtent3D     minImageTransferGranularity;
} CtsQueueFamilyProperties;

#ifdef __cplusplus
}
#endif

