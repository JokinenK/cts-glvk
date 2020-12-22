#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsUnmapMemory {
    CtsCmdBase base;
    CtsDevice device;
    CtsDeviceMemory memory;
} CtsUnmapMemory;

#ifdef __cplusplus
}
#endif