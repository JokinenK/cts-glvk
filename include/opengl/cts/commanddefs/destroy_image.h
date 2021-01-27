#pragma once

#include <cts/allocator.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyImage {
    CtsCmdBase base;
    CtsDevice device;
    CtsImage image;
    const CtsAllocationCallbacks* pAllocator;
} CtsDestroyImage;

#ifdef __cplusplus
}
#endif