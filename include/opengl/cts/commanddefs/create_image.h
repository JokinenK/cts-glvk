#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateImage {
    CtsCmdBase base;
    CtsDevice device;
    const CtsImageCreateInfo* pCreateInfo;
    const CtsAllocationCallbacks* pAllocator;
    CtsImage* pImage;
    CtsResult* pResult;
} CtsCreateImage;

#ifdef __cplusplus
}
#endif