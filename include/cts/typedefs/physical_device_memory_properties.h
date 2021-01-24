#pragma once

#include <cts/macros.h>
#include <cts/typedefs/memory_type.h>
#include <cts/typedefs/memory_heap.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPhysicalDeviceMemoryProperties {
    uint32_t        memoryTypeCount;
    CtsMemoryType   memoryTypes[CTS_MAX_MEMORY_TYPES];
    uint32_t        memoryHeapCount;
    CtsMemoryHeap   memoryHeaps[CTS_MAX_MEMORY_HEAPS];
} CtsPhysicalDeviceMemoryProperties;

#ifdef __cplusplus
}
#endif
