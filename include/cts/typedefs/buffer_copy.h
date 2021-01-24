#pragma once

#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBufferCopy {
    CtsDeviceSize   srcOffset;
    CtsDeviceSize   dstOffset;
    CtsDeviceSize   size;
} CtsBufferCopy;

#ifdef __cplusplus
}
#endif