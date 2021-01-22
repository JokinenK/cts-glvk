#pragma once

#include <stdint.h>
#include <cts/typedefs/buffer.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBufferViewImpl* CtsBufferView;

typedef struct CtsBufferViewCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsBuffer buffer;
    CtsFormat format;
    CtsDeviceSize offset;
    CtsDeviceSize range;
} CtsBufferViewCreateInfo;

#ifdef __cplusplus
}
#endif