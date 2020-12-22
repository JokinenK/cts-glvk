#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyBuffer {
    CtsCmdBase base;
    CtsDevice device;
    CtsBuffer buffer;
    const CtsAllocationCallbacks* allocator;
    CtsResult* result;
} CtsDestroyBuffer;

#ifdef __cplusplus
}
#endif