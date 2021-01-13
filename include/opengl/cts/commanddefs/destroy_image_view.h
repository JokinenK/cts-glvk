#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyImageView {
    CtsCmdBase base;
    CtsDevice device;
    CtsImageView imageView;
    const CtsAllocationCallbacks* pAllocator;
} CtsDestroyImageView;

#ifdef __cplusplus
}
#endif