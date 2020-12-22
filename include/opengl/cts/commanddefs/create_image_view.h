#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateImageView {
    CtsCmdBase base;
    CtsDevice device;
    const CtsImageViewCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsImageView* imageView;
    CtsResult* result;
} CtsCreateImageView;

#ifdef __cplusplus
}
#endif