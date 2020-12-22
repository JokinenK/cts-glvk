#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBindBufferMemory {
    CtsCmdBase base;
    CtsDevice device;
    CtsBuffer buffer;
    CtsDeviceMemory memory;
    CtsDeviceSize offset;
    CtsResult* result;
} CtsBindBufferMemory;

#ifdef __cplusplus
}
#endif