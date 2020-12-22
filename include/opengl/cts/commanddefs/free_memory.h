#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFreeMemory {
    CtsCmdBase base;
    CtsDevice device;
    CtsDeviceMemory memory;
    const CtsAllocationCallbacks* allocator;
} CtsFreeMemory;

#ifdef __cplusplus
}
#endif