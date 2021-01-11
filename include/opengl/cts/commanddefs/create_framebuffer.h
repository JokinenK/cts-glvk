#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateFramebuffer {
    CtsCmdBase base;
    CtsDevice device;
    const CtsFramebufferCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsFramebuffer* framebuffer;
    CtsResult* result;
} CtsCreateFramebuffer;

#ifdef __cplusplus
}
#endif