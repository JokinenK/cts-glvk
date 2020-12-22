#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateBuffer {
    CtsCmdBase base;
    CtsDevice device;
    const CtsBufferCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsBuffer* buffer;
    CtsResult* result;
} CtsCreateBuffer;

#ifdef __cplusplus
}
#endif