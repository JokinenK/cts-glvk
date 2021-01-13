#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsResetFences {
    CtsCmdBase base;
    CtsDevice device;
    uint32_t fenceCount;
    const CtsFence* pFences;
    CtsResult* pResult;
} CtsResetFences;

#ifdef __cplusplus
}
#endif