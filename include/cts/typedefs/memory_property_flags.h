#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsMemoryPropertyFlags;
typedef enum CtsMemoryPropertyFlagBits {
    CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x00000001,
    CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000002,
    CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00000004,
    CTS_MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00000008,
    CTS_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000010,
} CtsMemoryPropertyFlagBits;

#ifdef __cplusplus
}
#endif

