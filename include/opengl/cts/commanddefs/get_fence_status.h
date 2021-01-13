#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGetFenceStatus {
    CtsCmdBase base;
    CtsDevice device;
    CtsFence fence;
    CtsResult* pResult;
} CtsGetFenceStatus;

#ifdef __cplusplus
}
#endif