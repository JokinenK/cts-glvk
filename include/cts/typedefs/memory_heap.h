#pragma once

#include <cts/typedefs/device_size.h>
#include <cts/typedefs/memory_heap_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMemoryHeap {
    CtsDeviceSize size;
    CtsMemoryHeapFlags flags;
} CtsMemoryHeap;

#ifdef __cplusplus
}
#endif