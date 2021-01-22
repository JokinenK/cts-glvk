#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsMemoryHeapFlags;
typedef enum CtsMemoryHeapFlagBits {
    CTS_MEMORY_HEAP_DEVICE_LOCAL_BIT = 0x00000001,
} CtsMemoryHeapFlagBits;

#ifdef __cplusplus
}
#endif