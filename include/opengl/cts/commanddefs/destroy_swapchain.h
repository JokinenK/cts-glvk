#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySwapchain {
    CtsCmdBase base;
    CtsDevice device;
    CtsSwapchain swapchain;
    const CtsAllocationCallbacks* pAllocator;
} CtsDestroySwapchain;

#ifdef __cplusplus
}
#endif