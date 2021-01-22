#pragma once

#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMemoryRequirements {
    CtsDeviceSize size;
    CtsDeviceSize alignment;
    uint32_t memoryTypeBits;
} CtsMemoryRequirements;

#ifdef __cplusplus
}
#endif