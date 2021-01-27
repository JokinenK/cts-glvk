#pragma once

#include <cts/allocator.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateImageView {
    CtsCmdBase base;
    CtsDevice device;
    const CtsImageViewCreateInfo* pCreateInfo;
    const CtsAllocationCallbacks* pAllocator;
    CtsImageView* pImageView;
    CtsResult* pResult;
} CtsCreateImageView;

#ifdef __cplusplus
}
#endif