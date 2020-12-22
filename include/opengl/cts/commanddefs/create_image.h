#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateImage {
    CtsCmdBase base;
    CtsDevice device;
    const CtsImageCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsImage* image;
    CtsResult* result;
} CtsCreateImage;

#ifdef __cplusplus
}
#endif