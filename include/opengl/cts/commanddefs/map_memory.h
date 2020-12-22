#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMapMemory {
    CtsCmdBase base;
    CtsDevice device;
    CtsDeviceMemory memory;
    CtsDeviceSize offset;
    CtsDeviceSize size;
    CtsMemoryMapFlags flags;
    void** data;
    CtsResult* result;
} CtsMapMemory;

#ifdef __cplusplus
}
#endif