#pragma once

#include <stdint.h>
#include <cts/typedefs/device_memory.h>
#include <cts/typedefs/device_size.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMappedMemoryRange {
    CtsStructureType    sType;
    const void*         pNext;
    CtsDeviceMemory     memory;
    CtsDeviceSize       offset;
    CtsDeviceSize       size;
} CtsMappedMemoryRange;

#ifdef __cplusplus
}
#endif
