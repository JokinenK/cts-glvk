#pragma once

#include <stdint.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMemoryAllocateInfo {
    CtsStructureType sType;
    const void* next;
    CtsDeviceSize allocationSize;
    uint32_t memoryTypeIndex;
} CtsMemoryAllocateInfo;

#ifdef __cplusplus
}
#endif
