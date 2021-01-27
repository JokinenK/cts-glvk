#pragma once

#include <cts/allocator.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateFramebuffer {
    CtsCmdBase base;
    CtsDevice device;
    const CtsFramebufferCreateInfo* pCreateInfo;
    const CtsAllocationCallbacks* pAllocator;
    CtsFramebuffer* pFramebuffer;
    CtsResult* pResult;
} CtsCreateFramebuffer;

#ifdef __cplusplus
}
#endif