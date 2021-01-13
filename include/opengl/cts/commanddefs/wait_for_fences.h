#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsWaitForFences {
    CtsCmdBase base;
    CtsDevice device;
    uint32_t fenceCount;
    const CtsFence* pFences;
    CtsBool32 waitAll;
    uint64_t timeout;
    CtsResult* pResult;
} CtsWaitForFences;

#ifdef __cplusplus
}
#endif