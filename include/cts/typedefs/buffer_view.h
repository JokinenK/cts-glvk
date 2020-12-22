#pragma once

#include <stdint.h>
#include <cts/typedefs/buffer.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBufferView* CtsBufferView;

typedef struct CtsBufferViewCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsBuffer buffer;
    CtsAttribType format;
    CtsDeviceSize offset;
    CtsDeviceSize range;
} CtsBufferViewCreateInfo;

#ifdef __cplusplus
}
#endif