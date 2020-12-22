#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBuffer* CtsBuffer;

typedef struct CtsBufferCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsBufferType type;
    CtsBufferUsage usage;
    CtsDeviceSize size;
} CtsBufferCreateInfo;

#ifdef __cplusplus
}
#endif