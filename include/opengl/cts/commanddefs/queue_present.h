#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueuePresent {
    CtsCmdBase base;
    CtsQueue queue;
    const CtsPresentInfo* pPresentInfo;
    CtsResult* pResult;
} CtsQueuePresent;

#ifdef __cplusplus
}
#endif
