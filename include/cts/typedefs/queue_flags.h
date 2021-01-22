#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsQueueFlags;
typedef enum CtsQueueFlagBits {
    CTS_QUEUE_GRAPHICS_BIT = 0x00000001,
    CTS_QUEUE_COMPUTE_BIT = 0x00000002,
    CTS_QUEUE_TRANSFER_BIT = 0x00000004,
    CTS_QUEUE_SPARSE_BINDING_BIT = 0x00000008,
} CtsQueueFlagBits;

#ifdef __cplusplus
}
#endif

