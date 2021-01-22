#pragma once

#include <stdint.h>
#include <cts/typedefs/memory_property_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMemoryType {
    CtsMemoryPropertyFlags propertyFlags;
    uint32_t heapIndex;
} CtsMemoryType;

#ifdef __cplusplus
}
#endif